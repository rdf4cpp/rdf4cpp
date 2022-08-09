#include "OWL.hpp"

namespace rdf4cpp::rdf::namespaces {

OWL::OWL(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces