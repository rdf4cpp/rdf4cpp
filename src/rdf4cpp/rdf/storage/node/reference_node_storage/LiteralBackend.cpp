#include "LiteralBackend.hpp"

#include <sstream>
#include <tuple>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

LiteralBackend::LiteralBackend(std::string_view lexical, identifier::NodeID dataType, std::string_view langTag) noexcept
    : datatype_id_(dataType),
      lexical(lexical),
      lang_tag(langTag),
      hash_(View(*this).hash()) {}
LiteralBackend::LiteralBackend(view::LiteralBackendView view) noexcept
    : datatype_id_(view.datatype_id),
      lexical(view.lexical_form),
      lang_tag(view.language_tag),
      hash_(View(*this).hash()){};

std::partial_ordering LiteralBackend::operator<=>(LiteralBackend const &other) const noexcept {
    return std::tie(this->datatype_id_, this->lexical, this->lang_tag) <=> std::tie(other.datatype_id_, other.lexical, other.lang_tag);
}

bool LiteralBackend::operator==(LiteralBackend const &other) const noexcept {
    return std::tie(this->datatype_id_, this->lexical, this->lang_tag) == std::tie(other.datatype_id_, other.lexical, other.lang_tag);
}
std::partial_ordering LiteralBackend::operator<=>(std::unique_ptr<LiteralBackend> const &other) const noexcept {
    if (other)
        return *this <=> *other;
    else
        return std::partial_ordering::greater;
}
std::string_view LiteralBackend::language_tag() const noexcept {
    return lang_tag;
}
const identifier::NodeID &LiteralBackend::datatype_id() const noexcept {
    return datatype_id_;
}
std::string_view LiteralBackend::lexical_form() const noexcept {
    return lexical;
}
LiteralBackend::operator view::LiteralBackendView() const noexcept {
    return {.datatype_id = datatype_id(),
            .lexical_form = lexical_form(),
            .language_tag = language_tag()};
}

std::partial_ordering operator<=>(std::unique_ptr<LiteralBackend> const &self, std::unique_ptr<LiteralBackend> const &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage