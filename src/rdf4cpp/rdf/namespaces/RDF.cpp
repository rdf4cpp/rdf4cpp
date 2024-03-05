#include "RDF.hpp"

#include <charconv>

namespace rdf4cpp::rdf::namespaces {

RDF::RDF(storage::DynNodeStorage node_storage) : ClosedNamespace{prefix, suffixes, node_storage} {}

IRI RDF::operator+(std::string_view suffix) const {
    if (suffix.starts_with('_')) {
        size_t id;
        auto from_chars_result = std::from_chars(suffix.data() + 1, suffix.data() + suffix.size(), id);
        if (from_chars_result.ec == std::errc() and from_chars_result.ptr == suffix.data() + suffix.size()) {
            std::string normalized_suffix = std::string{"_"} + std::to_string(id);
            return Namespace::operator+(normalized_suffix); // add rdf:_<id> which is also a valid member of this namespace
        } else {
            std::ostringstream oss;
            oss << "Resource " << suffix << " does not exist within the vocabulary " << namespace_iri_ << '.';
            throw std::runtime_error{oss.str()};
        }
    }
    return ClosedNamespace::operator+(suffix);
}

}  // namespace rdf4cpp::rdf::namespaces