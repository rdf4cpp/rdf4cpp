#include "BNodeBackendView.hpp"

#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {
std::string BNodeBackendView::n_string() const noexcept {
    return "_:" + std::string{identifier};
}
size_t BNodeBackendView::hash() const noexcept {
    return std::hash<BNodeBackendView>()(*this);
}
}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::BNodeBackendView>::operator()(rdf4cpp::rdf::storage::node::view::BNodeBackendView const &x) const noexcept {
    using namespace rdf4cpp::rdf::storage::util;

    return rdf4cpp::rdf::storage::util::robin_hood::hash<std::array<size_t, 2>>{}(std::array<size_t, 2>{
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>()(x.identifier),
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::optional<rdf4cpp::rdf::util::WeakNodeScope>>{}(x.scope != nullptr ? std::optional{*x.scope} : std::nullopt)});
}
