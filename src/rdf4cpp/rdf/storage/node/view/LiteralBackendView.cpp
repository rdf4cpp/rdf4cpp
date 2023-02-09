#include "LiteralBackendView.hpp"

#include "rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp"

#include <array>

namespace rdf4cpp::rdf::storage::node::view {

size_t LexicalFormBackendView::hash() const noexcept {
    return util::robin_hood::hash<std::array<size_t, 3>>{}(
                std::array<size_t, 3>{
                        this->datatype_id.value(),
                        util::robin_hood::hash<std::string_view>{}(this->lexical_form),
                        util::robin_hood::hash<std::string_view>{}(this->language_tag)});
}

size_t AnyBackendView::hash() const noexcept {
    return util::robin_hood::hash<std::array<size_t, 2>>{}(
                std::array<size_t, 2>{
                    this->datatype.to_underlying(),
                    this->value.hash()});
}

size_t LiteralBackendView::hash() const noexcept {
    return std::visit([](auto const &x) {
        return x.hash();
    }, this->literal);
}

}  // namespace rdf4cpp::rdf::storage::node::view

size_t std::hash<rdf4cpp::rdf::storage::node::view::LiteralBackendView>::operator()(const rdf4cpp::rdf::storage::node::view::LiteralBackendView &x) const noexcept {
    return x.hash();
}
