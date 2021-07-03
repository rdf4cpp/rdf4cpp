#include "LiteralBackend.hpp"
#include <tuple>
namespace rdf4cpp::rdf::storage::node {

LiteralBackend::LiteralBackend(std::string lexical, const NodeID &dataType, std::string langTag)
    : datatype_id_(dataType),
      lexical(std::move(lexical)),
      lang_tag(std::move(langTag)) {}
std::strong_ordering LiteralBackend::operator<=>(const LiteralBackend &other) const noexcept {
    return std::tie(this->datatype_id_.node_id(), this->lexical, this->lang_tag) <=> std::tie(other.datatype_id_.node_id(), other.lexical, other.lang_tag);
}

std::string LiteralBackend::quote_lexical() const {
    // TODO: escape quotes (") in lexical + escape everything that needs to be escaped in N-Tripels/N-Quads
    return "\"" + lexical + "\"";
}
bool LiteralBackend::operator==(const LiteralBackend &) const noexcept {
    return std::tie(this->datatype_id_.node_id(), this->lexical, this->lang_tag) == std::tie(this->datatype_id_.node_id(), this->lexical, this->lang_tag);
};
}  // namespace rdf4cpp::rdf::storage::node