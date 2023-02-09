#ifndef RDF4CPP_LITERALBACKEND_HPP
#define RDF4CPP_LITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

class LiteralBackend {
    identifier::NodeID datatype_id_;
    std::string lexical;
    std::string lang_tag;
    size_t hash_;

public:
    using View = view::LexicalFormBackendView;
    LiteralBackend(std::string_view lexical, identifier::NodeID dataType, std::string_view langTag = "") noexcept;
    explicit LiteralBackend(view::LexicalFormBackendView view) noexcept;

    [[nodiscard]] std::string_view lexical_form() const noexcept;
    [[nodiscard]] identifier::NodeID datatype_id() const noexcept;
    [[nodiscard]] std::string_view language_tag() const noexcept;
    [[nodiscard]] size_t hash() const noexcept { return hash_; }

    explicit operator view::LexicalFormBackendView() const noexcept;
    explicit operator view::LiteralBackendView() const noexcept;
};


}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_LITERALBACKEND_HPP
