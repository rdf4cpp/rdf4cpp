#include "MADSRDF.hpp"

namespace rdf4cpp::rdf::namespaces {

MADSRDF::MADSRDF(storage::node::DynNodeStorage node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces