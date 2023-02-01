#include <rdf4cpp/rdf/util/BlankNodeIdScope.hpp>
#include <rdf4cpp/rdf/util/reference_backends/ReferenceBlankNodeIdScope.hpp>

namespace rdf4cpp::rdf::util {


BlankNodeIdScope::BlankNodeIdScope(IBlankNodeScopeBackend *backend, bool owned) noexcept : backend{backend}, owned{owned} {
}
BlankNodeIdScope::BlankNodeIdScope(BlankNodeIdScope::generator_type &generator)
    : BlankNodeIdScope{new ReferenceBlankNodeIdScope{generator}, true} {
}

BlankNodeIdScope::BlankNodeIdScope(BlankNodeIdScope &&other) noexcept : backend{other.backend}, owned{other.owned} {
    other.backend = nullptr;
}

BlankNodeIdScope::~BlankNodeIdScope() noexcept {
    if (this->owned && this->backend != nullptr) {
        delete this->backend;
    }
}

BlankNode BlankNodeIdScope::generate_bnode(NodeStorage &node_storage) {
    return this->backend->generate_bnode(node_storage);
}

IRI BlankNodeIdScope::generate_skolem_iri(std::string_view iri_prefix, NodeStorage &node_storage) {
    return this->backend->generate_skolem_iri(iri_prefix, node_storage);
}

BlankNode BlankNodeIdScope::try_get_bnode(std::string_view label) const noexcept {
    return this->backend->try_get_bnode(label);
}

IRI BlankNodeIdScope::try_get_skolem_iri(std::string_view label) const noexcept {
    return this->backend->try_get_skolem_iri(label);
}

BlankNode BlankNodeIdScope::get_or_generate_bnode(std::string_view label, NodeStorage &node_storage) {
    return this->backend->get_or_generate_bnode(label, node_storage);
}

IRI BlankNodeIdScope::get_or_generate_skolem_iri(std::string_view iri_prefix, std::string_view label, NodeStorage &node_storage) {
    return this->backend->get_or_generate_skolem_iri(iri_prefix, label, node_storage);
}

BlankNodeIdScope BlankNodeIdScope::subscope(std::string const &scope_name) noexcept {
    return BlankNodeIdScope{&this->backend->subscope(scope_name), false};
}

BlankNodeIdScope BlankNodeIdScope::subscope(std::string &&scope_name) noexcept {
    return BlankNodeIdScope{&this->backend->subscope(std::move(scope_name)), false};
}

}  //namespace rdf4cpp::rdf::util
