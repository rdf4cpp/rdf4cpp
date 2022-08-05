#include "XSD.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string XSD::NAMESPACE = "http://xmlns.com/XSD/0.1/";
XSD::XSD(Namespace::NodeStorage &node_storage) : Namespace(NAMESPACE, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces