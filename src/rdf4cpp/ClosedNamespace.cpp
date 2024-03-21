#include "ClosedNamespace.hpp"

namespace rdf4cpp {

IRI ClosedNamespace::operator+(std::string_view suffix) const {
    if (auto found = cache_.find(suffix); found != cache_.end()) {
        return IRI{storage::identifier::NodeBackendHandle{found->second, node_storage_}};
    } else {
        std::ostringstream oss;
        oss << "Resource " << suffix << " does not exist within the vocabulary " << namespace_iri_ << '.';
        throw std::runtime_error{oss.str()};
    }
}

void ClosedNamespace::clear() const {
    // this should not be cleared
}

}  // namespace rdf4cpp
