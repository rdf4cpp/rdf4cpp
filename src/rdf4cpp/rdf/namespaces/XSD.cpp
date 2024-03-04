#include "XSD.hpp"

namespace rdf4cpp::rdf::namespaces {

XSD::XSD(storage::node::DynNodeStorage node_storage) : Namespace(NAMESPACE, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces