#include "RDF.hpp"

namespace rdf4cpp::rdf::namespaces {

const std::string RDF::NAMESPACE = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
const std::vector<std::string> RDF::SUFFIXES = {"HTML", "langString", "PlainLiteral", "type", "Property", "Statement", "subject", "predicate", "object", "Bag", "Seq", "Alt", "value", "List", "nil", "first", "rest", "XMLLiteral", "JSON", "CompoundLiteral", "language", "direction"};
RDF::RDF(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}

}  // namespace rdf4cpp::rdf::namespaces