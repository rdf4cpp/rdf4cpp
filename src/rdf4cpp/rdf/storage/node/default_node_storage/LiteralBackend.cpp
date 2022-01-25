#include "LiteralBackend.hpp"
#include <tuple>
namespace rdf4cpp::rdf::storage::node::default_node_storage {

LiteralBackend::LiteralBackend(std::string_view lexical, const identifier::NodeID &dataType, std::string_view langTag) noexcept
    : datatype_id_(dataType),
      lexical(lexical),
      lang_tag(langTag) {}
std::strong_ordering LiteralBackend::operator<=>(const LiteralBackend &other) const noexcept {
    return std::tie(this->datatype_id_.node_id(), this->lexical, this->lang_tag) <=> std::tie(other.datatype_id_.node_id(), other.lexical, other.lang_tag);
}

std::string LiteralBackend::quote_lexical() const noexcept {
    // TODO: escape quotes (") in lexical + escape everything that needs to be escaped in N-Tripels/N-Quads
    return "\"" + lexical + "\"";
}
bool LiteralBackend::operator==(const LiteralBackend &other) const noexcept {
    return std::tie(this->datatype_id_.node_id(), this->lexical, this->lang_tag) == std::tie(other.datatype_id_.node_id(), other.lexical, other.lang_tag);
}
std::strong_ordering LiteralBackend::operator<=>(const std::unique_ptr<LiteralBackend> &other) const noexcept {
    if (other)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
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
LiteralBackend::operator handle::LiteralBackendView() const noexcept {
    return {.datatype_id = datatype_id(),
            .lexical_form = lexical_form(),
            .language_tag = language_tag()};
};
std::strong_ordering operator<=>(const std::unique_ptr<LiteralBackend> &self, const std::unique_ptr<LiteralBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage