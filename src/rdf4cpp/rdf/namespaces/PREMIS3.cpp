#include "PREMIS3.hpp"

namespace rdf4cpp::rdf::namespaces {

PREMIS3::PREMIS3(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces