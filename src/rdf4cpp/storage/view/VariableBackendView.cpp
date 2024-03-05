#include "VariableBackendView.hpp"

#include <dice/hash.hpp>

namespace rdf4cpp::storage::view {
size_t VariableBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(std::tie(is_anonymous, name));
}
}  // namespace rdf4cpp::storage::view

size_t std::hash<rdf4cpp::storage::view::VariableBackendView>::operator()(rdf4cpp::storage::view::VariableBackendView const &x) const noexcept {
    return x.hash();
}
