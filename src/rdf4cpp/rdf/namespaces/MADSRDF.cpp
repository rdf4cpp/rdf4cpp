#include <rdf4cpp/rdf/namespaces/MADSRDF.hpp>

namespace rdf4cpp::rdf::namespaces {

MADSRDF::MADSRDF(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces