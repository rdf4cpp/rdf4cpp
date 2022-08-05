#include "Namespace.hpp"

namespace rdf4cpp::rdf {

Namespace::Namespace(std::string_view namespace_iri, Namespace::NodeStorage &node_storage)
    : namespace_iri_(namespace_iri), node_storage_(node_storage) {}
std::string const &Namespace::names_space() const noexcept {
    return namespace_iri_;
}
const Namespace::NodeStorage &Namespace::node_storage() const noexcept {
    return node_storage_;
}
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
