#include <rdf4cpp/bnode_mngt/reference_backends/factory/SkolemIRIFactory.hpp>

namespace rdf4cpp::bnode_mngt {

SkolemIRIFactory::SkolemIRIFactory(std::string iri_prefix) noexcept : iri_prefix{std::move(iri_prefix)} {
}

storage::identifier::NodeBackendHandle SkolemIRIFactory::make_node(IIdGenerator &generator,
                                                                   [[maybe_unused]] NodeScope const *scope,
                                                                   storage::DynNodeStoragePtr node_storage) noexcept {

    size_t const buf_sz = generator.max_generated_id_size() + this->iri_prefix.size();
    auto buf = std::make_unique<char[]>(buf_sz);

    auto *end = std::copy(this->iri_prefix.begin(), this->iri_prefix.end(), buf.get());
    end = generator.generate_to_buf(end);

    auto const node_id = node_storage.find_or_make_id(storage::view::IRIBackendView{.identifier = std::string_view{buf.get(), static_cast<size_t>(end - buf.get())}});
    return storage::identifier::NodeBackendHandle{node_id, storage::identifier::RDFNodeType::IRI, node_storage};
}

}  //namespace rdf4cpp::bnode_mngt
