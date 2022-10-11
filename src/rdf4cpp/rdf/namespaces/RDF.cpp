#include "RDF.hpp"

#include <fmt/format.h>

#include <charconv>

namespace rdf4cpp::rdf::namespaces {

RDF::RDF(Namespace::NodeStorage &node_storage) : ClosedNamespace(NAMESPACE, SUFFIXES, node_storage) {}
IRI RDF::operator+(std::string_view suffix) const {
    if (suffix.starts_with('_')) {
        size_t id;
        auto from_chars_result = std::from_chars(suffix.data() + 1, suffix.data() + suffix.size(), id);
        if (from_chars_result.ec == std::errc() and from_chars_result.ptr == suffix.data() + suffix.size()) {
            std::string normalized_suffix = std::string{"_"} + std::to_string(id);
            return Namespace::operator+(normalized_suffix);
        } else {
            throw std::runtime_error{fmt::format("Resource {} does not exist within the vocabulary {}.", suffix, namespace_iri_)};
        }
    }
    return ClosedNamespace::operator+(suffix);
}

}  // namespace rdf4cpp::rdf::namespaces