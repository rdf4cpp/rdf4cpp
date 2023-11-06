#include <rdf4cpp/rdf/namespaces/BFLC.hpp>

namespace rdf4cpp::rdf::namespaces {

BFLC::BFLC(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces