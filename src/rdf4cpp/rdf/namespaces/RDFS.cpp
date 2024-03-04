#include "RDFS.hpp"

namespace rdf4cpp::rdf::namespaces {

RDFS::RDFS(storage::node::DynNodeStorage node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces