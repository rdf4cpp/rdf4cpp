#include "FallbackLiteralBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

FallbackLiteralBackend::FallbackLiteralBackend(identifier::NodeID const datatype_id,
                               std::string_view const lexical_form,
                               std::string_view const language_tag) noexcept : hash_{View{datatype_id, lexical_form, language_tag}.hash()},
                                                                               datatype_id{datatype_id},
                                                                               lexical_form{lexical_form},
                                                                               language_tag{language_tag} {
}

FallbackLiteralBackend::FallbackLiteralBackend(View const &view) noexcept : hash_{view.hash()},
                                                                            datatype_id{view.datatype_id},
                                                                            lexical_form{view.lexical_form},
                                                                            language_tag{view.language_tag} {
}

FallbackLiteralBackend::operator View() const noexcept {
    return View{.datatype_id = this->datatype_id,
                .lexical_form = this->lexical_form,
                .language_tag = this->language_tag};
}

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
