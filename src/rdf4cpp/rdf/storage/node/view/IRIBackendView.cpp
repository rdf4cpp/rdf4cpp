#include "IRIBackendView.hpp"

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {
std::string IRIBackendView::n_string() const noexcept {
    return "<" + std::string{identifier} + ">";
}
size_t IRIBackendView::hash() const noexcept {
    return std::hash<IRIBackendView>()(*this);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::IRIBackendView>::operator()(const rdf4cpp::rdf::storage::node::view::IRIBackendView &x) const noexcept {
    return dice::hash::DiceHashwyhash<std::string_view>()(x.identifier);
}