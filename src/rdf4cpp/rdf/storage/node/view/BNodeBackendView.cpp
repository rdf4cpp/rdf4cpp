#include "BNodeBackendView.hpp"

#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {
std::string BNodeBackendView::n_string() const noexcept {
    return "_:" + std::string{identifier};
}
size_t BNodeBackendView::hash() const noexcept {
    return std::hash<BNodeBackendView>()(*this);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::BNodeBackendView>::operator()(const rdf4cpp::rdf::storage::node::view::BNodeBackendView &x) const noexcept {
    using namespace rdf4cpp::rdf::storage::util;
    return robin_hood::hash<std::string_view>()(x.identifier) ^ x.scope.ptr_hash();
}