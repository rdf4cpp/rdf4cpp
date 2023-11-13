#include "BNodeBackendView.hpp"

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {
std::string BNodeBackendView::n_string() const noexcept {
    std::string buf;
    buf.reserve(identifier.size() + 2);
    buf.append("_:");
    buf.append(identifier);
    return buf;
}
size_t BNodeBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(identifier);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::BNodeBackendView>::operator()(rdf4cpp::rdf::storage::node::view::BNodeBackendView const &x) const noexcept {
    return x.hash();
}