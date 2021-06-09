#include "rdf4cpp/rdf/graph/node_manager/LiteralBackend.h"

namespace rdf4cpp::rdf::graph::node_manager {

LiteralBackend::LiteralBackend(std::string lexical, const ID &dataType, std::string langTag)
    : lexical(std::move(lexical)),
      datatype_id_(dataType),
      lang_tag(std::move(langTag)) {}
std::weak_ordering LiteralBackend::operator<=>(const LiteralBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::weak_ordering::greater;
}
std::string LiteralBackend::as_string([[maybe_unused]] bool quoting) const {
    // TODO: fix impl.
    return lexical;
}
}  // namespace rdf4cpp::rdf::graph::node_manager