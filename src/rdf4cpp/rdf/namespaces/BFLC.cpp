#include "BFLC.hpp"

namespace rdf4cpp::rdf::namespaces {

BFLC::BFLC(storage::DynNodeStorage node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces