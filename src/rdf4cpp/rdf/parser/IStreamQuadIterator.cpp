#include <rdf4cpp/rdf/parser/IStreamQuadIterator.hpp>
#include <rdf4cpp/rdf/parser/IStreamQuadIteratorSerdImpl.hpp>

namespace rdf4cpp::rdf::parser {

bool IStreamQuadIterator::is_at_end() const noexcept {
    return this->impl == nullptr || this->impl->is_at_end();
}

IStreamQuadIterator::IStreamQuadIterator() noexcept
    : IStreamQuadIterator{std::default_sentinel} {
}

IStreamQuadIterator::IStreamQuadIterator(std::default_sentinel_t) noexcept
    : impl{nullptr} {
}

IStreamQuadIterator::IStreamQuadIterator(void *stream, Source src, ParsingFlags flags, prefix_storage_type prefixes, storage::node::NodeStorage node_storage) noexcept
    : impl{std::make_unique<Impl>(stream, src, flags, std::move(prefixes), std::move(node_storage))} {
    ++*this;
}

IStreamQuadIterator::IStreamQuadIterator(std::istream &istream, ParsingFlags flags, prefix_storage_type prefixes, storage::node::NodeStorage node_storage) noexcept
    : IStreamQuadIterator{&istream, Source::make_istream_source(), flags, std::move(prefixes), std::move(node_storage)} {
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
