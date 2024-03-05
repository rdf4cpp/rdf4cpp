#ifndef RDF4CPP_RDFS_HPP
#define RDF4CPP_RDFS_HPP

#include <rdf4cpp/rdf/ClosedNamespace.hpp>
#include <rdf4cpp/rdf/namespaces/util/NamespaceInstance.hpp>

namespace rdf4cpp::rdf::namespaces {

/**
 * A ClosedNamespace for https://www.w3.org/TR/rdf-schema/
 */
class RDFS : public ClosedNamespace, public util::NamespaceInstance<RDFS> {
public:
    static constexpr std::string_view NAMESPACE = "http://www.w3.org/2000/01/rdf-schema#";
    static constexpr std::array<std::string_view, 30> SUFFIXES = {"Resource", "Class", "Literal", "Datatype", "range", "domain", "subClassOf", "subPropertyOf", "label", "comment", "Container", "ContainerMembershipProperty", "member", "seeAlso", "isDefinedBy", "subClassOf ", "Resource ", "Class ", "Literal ", "Datatype ", "subPropertyOf ", "domain ", "range ", "label ", "comment ", "Container ", "ContainerMembershipProperty ", "member ", "seeAlso ", "isDefinedBy"};
    explicit RDFS(storage::DynNodeStorage node_storage = storage::default_node_storage);
};

}  // namespace rdf4cpp::rdf::namespaces

#endif  //RDF4CPP_RDFS_HPP
