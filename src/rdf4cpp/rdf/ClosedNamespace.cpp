#include "ClosedNamespace.hpp"
#include <fmt/format.h>

namespace rdf4cpp::rdf {

IRI ClosedNamespace::operator+(std::string_view suffix) const {
    [[maybe_unused]] auto ns = this->node_storage().upgrade();
    if (auto found = cache_.find(suffix); found != cache_.end()) {
        return IRI{found->second};
    } else {
        throw std::runtime_error{fmt::format("Resource {} does not exist within the vocabulary {}.", suffix, namespace_iri_)};
    }
}
void ClosedNamespace::clear() const {
    // this should not be cleared
}
}  // namespace rdf4cpp::rdf