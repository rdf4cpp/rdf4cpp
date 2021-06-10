#include "LiteralBackend.h"

namespace rdf4cpp::rdf::graph::node_manager {

LiteralBackend::LiteralBackend(std::string lexical, const NodeID &dataType, std::string langTag)
    : datatype_id_(dataType),
      lexical(std::move(lexical)),
      lang_tag(std::move(langTag)) {}
std::strong_ordering LiteralBackend::operator<=>(const LiteralBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}

std::string LiteralBackend::quote_lexical() const {
    return "\"" + lexical + "\"";
};
}  // namespace rdf4cpp::rdf::graph::node_manager