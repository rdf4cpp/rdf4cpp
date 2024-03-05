#include "BNodeBackendView.hpp"

#include <dice/hash.hpp>
#include <rdf4cpp/bnode_mngt/NodeScope.hpp>

namespace rdf4cpp::rdf::storage::view {
size_t BNodeBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(identifier);
}
}  // namespace rdf4cpp::rdf::storage::view

size_t std::hash<rdf4cpp::rdf::storage::view::BNodeBackendView>::operator()(rdf4cpp::rdf::storage::view::BNodeBackendView const &x) const noexcept {
    return x.hash();
}
