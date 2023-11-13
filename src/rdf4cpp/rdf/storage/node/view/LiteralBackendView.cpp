#include "LiteralBackendView.hpp"

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::storage::node::view {

size_t LexicalFormLiteralBackendView::hash() const noexcept {
    return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(std::tie(datatype_id,
                                                                                             lexical_form,
                                                                                             language_tag));
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
