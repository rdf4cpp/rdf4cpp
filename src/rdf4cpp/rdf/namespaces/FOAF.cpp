#include <rdf4cpp/rdf/namespaces/FOAF.hpp>

namespace rdf4cpp::rdf::namespaces {

FOAF::FOAF(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces