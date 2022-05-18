#include "LiteralBackendView.hpp"

#include "rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp"

namespace rdf4cpp::rdf::storage::node::view {
size_t LiteralBackendView::hash() const noexcept {
    return std::hash<LiteralBackendView>()(*this);
}
};  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::LiteralBackendView>::operator()(const rdf4cpp::rdf::storage::node::view::LiteralBackendView &x) const noexcept {
    using namespace rdf4cpp::rdf::storage::util;
    return robin_hood::hash<std::array<size_t, 3>>()(
            std::array<size_t, 3>{
                    x.datatype_id.value(),
                    robin_hood::hash<std::string_view>()(x.lexical_form),
                    robin_hood::hash<std::string_view>()(x.language_tag)});
}