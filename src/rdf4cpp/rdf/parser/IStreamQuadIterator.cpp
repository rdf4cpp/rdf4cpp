#include <rdf4cpp/rdf/parser/IStreamQuadIterator.hpp>
#include <rdf4cpp/rdf/parser/private/IStreamQuadIteratorSerdImpl.hpp>

namespace rdf4cpp::rdf::parser {

template<ByteSource Src>
bool IStreamQuadIterator<Src>::is_at_end() const noexcept {
    return this->impl == nullptr || this->impl->is_at_end();
}

template<ByteSource Src>
IStreamQuadIterator<Src>::IStreamQuadIterator() noexcept
    : IStreamQuadIterator{std::default_sentinel} {
}

template<ByteSource Src>
IStreamQuadIterator<Src>::IStreamQuadIterator(std::default_sentinel_t) noexcept
    : impl{nullptr} {
}

template<ByteSource Src>
IStreamQuadIterator<Src>::IStreamQuadIterator(Src src, ParsingFlags const flags, storage::node::NodeStorage &node_storage) noexcept
    : impl{std::make_unique<Impl>(std::move(src), flags.contains(ParsingFlag::Strict), flags.contains(ParsingFlag::StopOnFirstError), node_storage)} {

    ++*this;
}

template<ByteSource Src>
IStreamQuadIterator<Src>::~IStreamQuadIterator() noexcept = default;

template<ByteSource Src>
typename IStreamQuadIterator<Src>::reference IStreamQuadIterator<Src>::operator*() const noexcept {
    return this->cur;
}

template<ByteSource Src>
typename IStreamQuadIterator<Src>::pointer IStreamQuadIterator<Src>::operator->() const noexcept {
    return &this->cur;
}

template<ByteSource Src>
IStreamQuadIterator<Src> &IStreamQuadIterator<Src>::operator++() {
    if (auto maybe_value = this->impl->next(); maybe_value.has_value()) {
        this->cur = std::move(*maybe_value);
    }

    return *this;
}

template<ByteSource Src>
bool IStreamQuadIterator<Src>::operator==(IStreamQuadIterator const &other) const noexcept {
    return (this->is_at_end() && other.is_at_end()) || this->impl == other.impl;
}

template<ByteSource Src>
bool IStreamQuadIterator<Src>::operator!=(IStreamQuadIterator const &other) const noexcept {
    return !(*this == other);
}

template struct IStreamQuadIterator<IStreamByteSource>;
template struct IStreamQuadIterator<FileByteSource>;

} // namespace rdf4cpp::rdf::parser
