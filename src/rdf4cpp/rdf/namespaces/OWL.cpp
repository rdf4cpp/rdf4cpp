#include "OWL.hpp"

namespace rdf4cpp::rdf::namespaces {

OWL::OWL(storage::DynNodeStorage node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces