#include "BNodeBackendView.hpp"
#include <rdf4cpp/rdf/bnode_mngt/NodeScope.hpp>

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {
size_t BNodeBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(identifier);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::BNodeBackendView>::operator()(rdf4cpp::rdf::storage::node::view::BNodeBackendView const &x) const noexcept {
    return x.hash();
}
