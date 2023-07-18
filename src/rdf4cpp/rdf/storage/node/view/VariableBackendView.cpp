#include "VariableBackendView.hpp"

#include <dice/hash.hpp>

#include <array>

namespace rdf4cpp::rdf::storage::node::view {
std::string VariableBackendView::n_string() const noexcept {
    if (is_anonymous)
        return "_:" + std::string{name};
    else
        return "?" + std::string{name};
}
size_t VariableBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(std::make_tuple(is_anonymous, name));
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::VariableBackendView>::operator()(rdf4cpp::rdf::storage::node::view::VariableBackendView const &x) const noexcept {
    return x.hash();
}