#include "Namespace.hpp"

namespace rdf4cpp::rdf {
rdf4cpp::rdf::IRI rdf4cpp::rdf::Namespace::operator+(std::string const &suffix) {
    if (auto found = cache_.find(suffix); found != cache_.end()) {
        return IRI(found->second);
    } else {
        IRI iri{namespace_iri_ + suffix, node_storage_};
        cache_.insert({{suffix, iri.backend_handle()}});
        return iri;
    }
}
void Namespace::clear() {
    cache_.clear();
    cache_.rehash(0);
}
}  // namespace rdf4cpp::rdf
