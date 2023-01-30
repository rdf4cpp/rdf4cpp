#include <rdf4cpp/rdf/util/BlankNodeIdGenerator.hpp>
#include <rdf4cpp/rdf/util/private/BlankNodeIdGeneratorImpl.hpp>

#include <array>
#include <algorithm>

namespace rdf4cpp::rdf::util {

BlankNodeIdGenerator::BlankNodeIdGenerator(std::unique_ptr<Impl> &&impl) : impl{std::move(impl)} {
}

BlankNodeIdGenerator::BlankNodeIdGenerator(BlankNodeIdGenerator &&other) noexcept = default;
BlankNodeIdGenerator::~BlankNodeIdGenerator() noexcept = default;

BlankNodeIdGenerator BlankNodeIdGenerator::from_seed(uint64_t const seed) {
    return BlankNodeIdGenerator{std::make_unique<Impl>(seed)};
}

BlankNodeIdGenerator BlankNodeIdGenerator::from_entropy() {
    return BlankNodeIdGenerator{std::make_unique<Impl>()};
}

std::string BlankNodeIdGenerator::generate_id() {
    std::string buf;
    this->impl->generate_to_string(buf);
    return buf;
}

BlankNode BlankNodeIdGenerator::generated_bnode(BlankNodeIdGenerator::NodeStorage &node_storage) {
    std::array<char, Impl::generated_id_len> buf;
    this->impl->generate_to_buf(buf.data());

    return BlankNode{std::string_view{buf.data(), buf.size()}, node_storage};
}

IRI BlankNodeIdGenerator::generate_skolem_iri(std::string_view iri_prefix, BlankNodeIdGenerator::NodeStorage &node_storage) {
    size_t const buf_sz = iri_prefix.size() + Impl::generated_id_len;
    auto iri_buf = std::make_unique<char[]>(buf_sz);

    std::copy(iri_prefix.begin(), iri_prefix.end(), iri_buf.get());
    this->impl->generate_to_buf(iri_buf.get() + iri_prefix.size());

    return IRI{std::string_view{iri_buf.get(), buf_sz}, node_storage};
}

BlankNodeIdScope BlankNodeIdGenerator::scope(NodeStorage &node_storage) {
    return BlankNodeIdScope{*this, node_storage};
}

BlankNodeIdGenerator &BlankNodeIdGenerator::default_instance() {
    static BlankNodeIdGenerator instance = BlankNodeIdGenerator::from_entropy();
    return instance;
}

size_t BlankNodeIdGenerator::generated_id_length() noexcept {
    return Impl::generated_id_len;
}

}  //namespace rdf4cpp::rdf::util
