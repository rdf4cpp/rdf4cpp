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
    return std::hash<VariableBackendView>()(*this);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::VariableBackendView>::operator()(const rdf4cpp::rdf::storage::node::view::VariableBackendView &x) const noexcept {
    return dice::hash::DiceHashwyhash<std::array<size_t, 2>>()(
            std::array<size_t, 2>{
                    x.is_anonymous,
                    dice::hash::DiceHashwyhash<std::string_view>()(x.name)});
}