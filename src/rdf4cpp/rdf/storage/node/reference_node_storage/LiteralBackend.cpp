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
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage