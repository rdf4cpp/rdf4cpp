#ifndef RDF4CPP_FALLBACKLITERALBACKEND_HPP
#define RDF4CPP_FALLBACKLITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct FallbackLiteralBackend {
    using View = view::LexicalFormLiteralBackendView;
    using Id = identifier::LiteralID;

    size_t hash;
    identifier::NodeID datatype_id;
    ConstString lexical_form;
    ConstString language_tag;
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

    static identifier::NodeID to_node_id(Id const id, View const view) noexcept {
        return identifier::NodeID{id, iri_node_id_to_literal_type(view.datatype_id)};
    }

    static Id to_backend_id(identifier::NodeID const id) noexcept {
        return id.literal_id();
    }
};

}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_FALLBACKLITERALBACKEND_HPP
