#ifndef RDF4CPP_FALLBACKLITERALBACKEND_HPP
#define RDF4CPP_FALLBACKLITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

#include <string>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct FallbackLiteralBackend {
    using View = view::LexicalFormLiteralBackendView;

    size_t hash;
    identifier::NodeID datatype_id;
    std::string lexical_form;
    std::string language_tag;
    bool needs_escape;

    explicit FallbackLiteralBackend(View const &view) noexcept : hash{view.hash()},
                                                                 datatype_id{view.datatype_id},
                                                                 lexical_form{view.lexical_form},
                                                                 language_tag{view.language_tag},
                                                                 needs_escape{view.needs_escape} {
    }

    explicit operator View() const noexcept {
        return View{.datatype_id = datatype_id,
                    .lexical_form = lexical_form,
                    .language_tag = language_tag,
                    .needs_escape = needs_escape};
    }
};

}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_FALLBACKLITERALBACKEND_HPP
