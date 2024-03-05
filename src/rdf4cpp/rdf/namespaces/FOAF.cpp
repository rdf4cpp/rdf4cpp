#include "FOAF.hpp"

namespace rdf4cpp::rdf::namespaces {

FOAF::FOAF(storage::DynNodeStorage node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces