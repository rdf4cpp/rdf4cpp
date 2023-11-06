#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {

std::string VariableBackendView::n_string() const noexcept {
    std::string buf;
    buf.reserve(name.size() + 2);

    if (is_anonymous) {
        buf.append("_:");
    } else {
        buf.push_back('?');
    }

    buf.append(name);
    return buf;
}

size_t VariableBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(std::tie(is_anonymous, name));
}

}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::VariableBackendView>::operator()(rdf4cpp::rdf::storage::node::view::VariableBackendView const &x) const noexcept {
    return x.hash();
}
