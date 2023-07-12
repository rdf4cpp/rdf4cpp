#include "LiteralBackendView.hpp"

#include <dice/hash.hpp>

#include <array>

namespace rdf4cpp::rdf::storage::node::view {

size_t LexicalFormLiteralBackendView::hash() const noexcept {
    return dice::hash::DiceHashwyhash<std::array<size_t, 3>>{}(
                std::array<size_t, 3>{
                        this->datatype_id.value(),
                        util::robin_hood::hash<std::string_view>{}(this->lexical_form),
                        util::robin_hood::hash<std::string_view>{}(this->language_tag)});
}

LiteralBackendView::LiteralBackendView(ValueLiteralBackendView const &any) : inner{any} {}
LiteralBackendView::LiteralBackendView(ValueLiteralBackendView &&any) noexcept : inner{std::move(any)} {}
LiteralBackendView::LiteralBackendView(LexicalFormLiteralBackendView const &lexical) noexcept : inner{lexical} {}

bool LiteralBackendView::is_lexical() const noexcept {
    return this->inner.index() == 0;
}
bool LiteralBackendView::is_value() const noexcept {
    return this->inner.index() == 1;
}
LexicalFormLiteralBackendView const &LiteralBackendView::get_lexical() const {
    return std::get<LexicalFormLiteralBackendView>(this->inner);
}
ValueLiteralBackendView const &LiteralBackendView::get_value() const & {
    return std::get<ValueLiteralBackendView>(this->inner);
}
ValueLiteralBackendView &&LiteralBackendView::get_value() && {
    return std::get<ValueLiteralBackendView>(std::move(this->inner));
}

}  // namespace rdf4cpp::rdf::storage::node::view
