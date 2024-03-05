#include "IRIBackendView.hpp"

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::view {
size_t IRIBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(identifier);
}
}  // namespace rdf4cpp::rdf::storage::view

size_t std::hash<rdf4cpp::rdf::storage::view::IRIBackendView>::operator()(rdf4cpp::rdf::storage::view::IRIBackendView const &x) const noexcept {
    return x.hash();
}