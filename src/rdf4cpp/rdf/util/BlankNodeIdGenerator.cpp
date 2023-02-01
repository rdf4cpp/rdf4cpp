#include <rdf4cpp/rdf/util/BlankNodeIdGenerator.hpp>
#include <rdf4cpp/rdf/util/reference_backends/RandomIdGeneratorBackend.hpp>

#include <array>
#include <algorithm>

namespace rdf4cpp::rdf::util {

BlankNodeIdGenerator::BlankNodeIdGenerator(std::unique_ptr<IBlankNodeIdGeneratorBackend> &&impl) : impl{std::move(impl)} {
}

BlankNodeIdGenerator::BlankNodeIdGenerator(BlankNodeIdGenerator &&other) noexcept = default;
BlankNodeIdGenerator::~BlankNodeIdGenerator() noexcept = default;

BlankNodeIdGenerator BlankNodeIdGenerator::with_backend(std::unique_ptr<IBlankNodeIdGeneratorBackend> backend) {
    return BlankNodeIdGenerator{std::move(backend)};
}

BlankNodeIdGenerator BlankNodeIdGenerator::from_seed(uint64_t const seed) {
    return BlankNodeIdGenerator{std::make_unique<RandomBlankNodeIdGenerator>(seed)};
}

BlankNodeIdGenerator BlankNodeIdGenerator::from_entropy() {
    return BlankNodeIdGenerator{std::make_unique<RandomBlankNodeIdGenerator>()};
}

std::string BlankNodeIdGenerator::generate_id() {
    std::string buf;
    this->impl->generate_to_string(buf);
    return buf;
}

BlankNode BlankNodeIdGenerator::generated_bnode(BlankNodeIdGenerator::NodeStorage &node_storage) {
    size_t const buf_sz = this->max_generated_id_size();
    auto buf = std::make_unique<char[]>(buf_sz);

    auto *end = this->impl->generate_to_buf(buf.get());

    return BlankNode{std::string_view{buf.get(), static_cast<size_t>(end - buf.get())}, node_storage};
}

IRI BlankNodeIdGenerator::generate_skolem_iri(std::string_view iri_prefix, BlankNodeIdGenerator::NodeStorage &node_storage) {
    size_t const buf_sz = iri_prefix.size() + this->max_generated_id_size();
    auto iri_buf = std::make_unique<char[]>(buf_sz);

    std::copy(iri_prefix.begin(), iri_prefix.end(), iri_buf.get());
    auto *end = this->impl->generate_to_buf(iri_buf.get() + iri_prefix.size());

    return IRI{std::string_view{iri_buf.get(), static_cast<size_t>(end - iri_buf.get())}, node_storage};
}

BlankNodeIdScope BlankNodeIdGenerator::scope() {
    return BlankNodeIdScope{*this};
}

BlankNodeIdGenerator &BlankNodeIdGenerator::default_instance() {
    static BlankNodeIdGenerator instance = BlankNodeIdGenerator::from_entropy();
    return instance;
}

size_t BlankNodeIdGenerator::max_generated_id_size() const noexcept {
    return this->impl->max_generated_id_size();
}

}  //namespace rdf4cpp::rdf::util
