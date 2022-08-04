#include "RDFS.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string RDFS::NAMESPACE = "http://www.w3.org/2000/01/rdf-schema#";
const std::vector<std::string> RDFS::SUFFIXES = {"Resource", "Class", "Literal", "Datatype", "range", "domain", "subClassOf", "subPropertyOf", "label", "comment", "Container", "ContainerMembershipProperty", "member", "seeAlso", "isDefinedBy", "subClassOf ", "Resource ", "Class ", "Literal ", "Datatype ", "subPropertyOf ", "domain ", "range ", "label ", "comment ", "Container ", "ContainerMembershipProperty ", "member ", "seeAlso ", "isDefinedBy"};
RDFS::RDFS(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces