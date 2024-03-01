#include "IRIBackendView.hpp"

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {
size_t IRIBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(identifier);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::IRIBackendView>::operator()(rdf4cpp::rdf::storage::node::view::IRIBackendView const &x) const noexcept {
    return x.hash();
}