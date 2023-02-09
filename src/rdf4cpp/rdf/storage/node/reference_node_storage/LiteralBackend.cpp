#include "LiteralBackend.hpp"

#include <sstream>
#include <tuple>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

LiteralBackend::LiteralBackend(std::string_view lexical, identifier::NodeID dataType, std::string_view langTag) noexcept
    : datatype_id_{dataType},
      lexical{lexical},
      lang_tag{langTag},
      hash_{View(*this).hash()} {
}

LiteralBackend::LiteralBackend(view::LexicalFormBackendView view) noexcept : datatype_id_{view.datatype_id},
                                                                             lexical{view.lexical_form},
                                                                             lang_tag{view.language_tag},
                                                                             hash_{view.hash()} {
}

std::string_view LiteralBackend::language_tag() const noexcept {
    return lang_tag;
}

identifier::NodeID LiteralBackend::datatype_id() const noexcept {
    return datatype_id_;
}

std::string_view LiteralBackend::lexical_form() const noexcept {
    return lexical;
}

LiteralBackend::operator view::LiteralBackendView() const noexcept {
    return view::LiteralBackendView{
        .literal = static_cast<view::LexicalFormBackendView>(*this)};
}

LiteralBackend::operator view::LexicalFormBackendView() const noexcept {
    return view::LexicalFormBackendView{
            .datatype_id = this->datatype_id_,
            .lexical_form = this->lexical,
            .language_tag = this->lang_tag};
}

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
