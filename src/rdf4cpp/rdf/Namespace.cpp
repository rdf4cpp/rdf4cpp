#include "Namespace.hpp"
#include <fmt/format.h>

namespace rdf4cpp::rdf {

Namespace::Namespace(std::string_view namespace_iri, Namespace::NodeStorage &node_storage)
    : namespace_iri_(namespace_iri), node_storage_(node_storage.downgrade()) {}
std::string_view Namespace::name_space() const noexcept {
    return namespace_iri_;
}
Namespace::WeakNodeStorage Namespace::node_storage() const {
    return node_storage_;
}
rdf4cpp::rdf::IRI rdf4cpp::rdf::Namespace::operator+(std::string_view suffix) const {
    auto nodes = node_storage_.upgrade();

    if (auto found = cache_.find(suffix); found != cache_.end()) {
        return IRI{found->second};
    } else {
        IRI iri{fmt::format("{}{}", namespace_iri_, suffix), nodes};
        cache_.emplace(suffix, iri.backend_handle());
        return iri;
    }
}
void Namespace::clear() const {
    cache_.clear();
    cache_.rehash(0);
}

}  // namespace rdf4cpp::rdf
