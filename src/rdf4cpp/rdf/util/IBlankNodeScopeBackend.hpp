#ifndef RDF4CPP_IBLANKNODESCOPEBACKEND_HPP
#define RDF4CPP_IBLANKNODESCOPEBACKEND_HPP

#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>

namespace rdf4cpp::rdf::util {

struct IBlankNodeScopeBackend {
    using NodeStorage = storage::node::NodeStorage;

    virtual ~IBlankNodeScopeBackend() = default;

    [[nodiscard]] virtual BlankNode generate_bnode(NodeStorage &node_storage) = 0;
    [[nodiscard]] virtual IRI generate_skolem_iri(std::string_view iri_prefix, NodeStorage &node_storage) = 0;

    [[nodiscard]] virtual BlankNode try_get_bnode(std::string_view label) const noexcept = 0;
    [[nodiscard]] virtual IRI try_get_skolem_iri(std::string_view label) const noexcept = 0;

    [[nodiscard]] virtual BlankNode get_or_generate_bnode(std::string_view label, NodeStorage &node_storage) = 0;
    [[nodiscard]] virtual IRI get_or_generate_skolem_iri(std::string_view iri_prefix, std::string_view label, NodeStorage &node_storage) = 0;

    [[nodiscard]] virtual IBlankNodeScopeBackend &subscope(std::string scope_name) noexcept = 0;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_IBLANKNODESCOPEBACKEND_HPP
