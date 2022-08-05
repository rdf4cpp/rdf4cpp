#include "RDF.hpp"

#include <fmt/format.h>

#include <charconv>

namespace rdf4cpp::rdf::namespaces {

const std::string RDF::NAMESPACE = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
const std::vector<std::string> RDF::SUFFIXES = {"HTML", "langString", "PlainLiteral", "type", "Property", "Statement", "subject", "predicate", "object", "Bag", "Seq", "Alt", "value", "List", "nil", "first", "rest", "XMLLiteral", "JSON", "CompoundLiteral", "language", "direction"};
RDF::RDF(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}
IRI RDF::operator+(const std::string &suffix) {
    if (suffix.starts_with('_')) {
        size_t id;
        auto from_chars_result = std::from_chars(suffix.data() + 1, suffix.data() + suffix.size(), id);
        if (from_chars_result.ec == std::errc()) {
            std::string normalized_suffix = std::string{"_"} + std::to_string(id);
            return Namespace::operator+(normalized_suffix);
        } else {
            throw std::runtime_error{fmt::format("Resource {} does not exist within the vocabulary {}.", suffix, namespace_iri_)};
        }
    }
    return ClosedNamespace::operator+(suffix);
}

}  // namespace rdf4cpp::rdf::namespaces