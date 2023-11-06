#include <rdf4cpp/rdf/namespaces/RDFS.hpp>

namespace rdf4cpp::rdf::namespaces {

RDFS::RDFS(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces