#include <rdf4cpp/rdf/parser/IStreamQuadIterator.hpp>
#include <rdf4cpp/rdf/parser/IStreamQuadIteratorSerdImpl.hpp>

namespace rdf4cpp::rdf::parser {

/**
 * Adaptor function so that serd can read from std::istreams.
 * Matches the interface of SerdSource/fread
 *
 * @param buf pointer to buffer to be written to
 * @param elem_size sizeof each element being written, guaranteed to always be 1
 * @param count number of elements to write
 * @param voided_self pointer to std::istream cast to void *
 */
static size_t istream_read(void *buf, [[maybe_unused]] size_t elem_size, size_t count, void *voided_self) noexcept {
    assert(elem_size == 1);

    auto *self = reinterpret_cast<std::istream *>(voided_self);
    self->read(static_cast<char *>(buf), static_cast<std::streamsize>(count));
    return self->gcount();
}

/**
 * Adaptor function for serd to check if an std::istream is ok
 * Matches the interface of SerdStreamErrorFunc
 *
 * @param voided_self pointer to std::istream cast to void *
 */
static int istream_error(void *voided_self) noexcept {
    auto *self = reinterpret_cast<std::istream *>(voided_self);
    return *self ? 0 : 1;
}

bool IStreamQuadIterator::is_at_end() const noexcept {
    return this->impl == nullptr || this->impl->is_at_end();
}

IStreamQuadIterator::IStreamQuadIterator() noexcept
    : IStreamQuadIterator{std::default_sentinel} {
}

IStreamQuadIterator::IStreamQuadIterator(std::default_sentinel_t) noexcept
    : impl{nullptr} {
}

IStreamQuadIterator::IStreamQuadIterator(void *stream,
                                         ReadFunc read,
                                         ErrorFunc error,
                                         ParsingFlags flags,
                                         prefix_storage_type prefixes,
                                         storage::node::NodeStorage node_storage) noexcept
    : impl{std::make_unique<Impl>(stream, read, error, flags, std::move(prefixes), std::move(node_storage))} {
    ++*this;
}

IStreamQuadIterator::IStreamQuadIterator(std::istream &istream, ParsingFlags flags, prefix_storage_type prefixes, storage::node::NodeStorage node_storage) noexcept
    : IStreamQuadIterator{&istream, &istream_read, &istream_error, flags, std::move(prefixes), std::move(node_storage)} {
}

IStreamQuadIterator::IStreamQuadIterator(IStreamQuadIterator &&other) noexcept = default;

IStreamQuadIterator::~IStreamQuadIterator() noexcept = default;

typename IStreamQuadIterator::reference IStreamQuadIterator::operator*() const noexcept {
    return this->cur;
}

typename IStreamQuadIterator::pointer IStreamQuadIterator::operator->() const noexcept {
    return &this->cur;
}

IStreamQuadIterator &IStreamQuadIterator::operator++() {
    if (auto maybe_value = this->impl->next(); maybe_value.has_value()) {
        this->cur = std::move(*maybe_value);
    }

    return *this;
}

bool IStreamQuadIterator::operator==(IStreamQuadIterator const &other) const noexcept {
    return (this->is_at_end() && other.is_at_end()) || this->impl == other.impl;
}

bool IStreamQuadIterator::operator!=(IStreamQuadIterator const &other) const noexcept {
    return !(*this == other);
}

bool IStreamQuadIterator::operator==(std::default_sentinel_t) const noexcept {
    return this->is_at_end();
}

bool IStreamQuadIterator::operator!=(std::default_sentinel_t) const noexcept {
    return !this->is_at_end();
}

}  // namespace rdf4cpp::rdf::parser
