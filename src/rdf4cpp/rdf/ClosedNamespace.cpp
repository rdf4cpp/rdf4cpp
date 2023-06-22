#include "ClosedNamespace.hpp"

namespace rdf4cpp::rdf {

IRI ClosedNamespace::operator+(std::string_view suffix) const {
    [[maybe_unused]] auto ns = this->node_storage().upgrade();
    if (auto found = cache_.find(suffix); found != cache_.end()) {
        return IRI{found->second};
    } else {
        std::ostringstream oss;
        oss << "Resource " << suffix << " does not exist within the vocabulary " << namespace_iri_ << '.';
        throw std::runtime_error{oss.str()};
    }
}
void ClosedNamespace::clear() const {
    // this should not be cleared
}
}  // namespace rdf4cpp::rdf