#include "LiteralBackend.hpp"
#include <tuple>
namespace rdf4cpp::rdf::storage::node {

LiteralBackend::LiteralBackend(std::string lexical, const NodeID &dataType, std::string langTag) noexcept
    : datatype_id_(dataType),
      lexical(std::move(lexical)),
      lang_tag(std::move(langTag)) {}
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
const std::string &LiteralBackend::language_tag() const noexcept {
    return lang_tag;
}
const NodeID &LiteralBackend::datatype_id() const noexcept {
    return datatype_id_;
}
const std::string &LiteralBackend::lexical_form() const noexcept {
    return lexical;
};
std::strong_ordering operator<=>(const std::unique_ptr<LiteralBackend> &self, const std::unique_ptr<LiteralBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node