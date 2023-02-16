#ifndef RDF4CPP_FALLBACKLITERALBACKEND_HPP
#define RDF4CPP_FALLBACKLITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>

#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct FallbackLiteralBackend {
    using View = view::LexicalFormLiteralBackendView;
private:
    size_t hash_;

public:
    identifier::NodeID datatype_id;
    std::string lexical_form;
    std::string language_tag;

    FallbackLiteralBackend(identifier::NodeID datatype_id, std::string_view lexical_form, std::string_view language_tag = "") noexcept;
    explicit FallbackLiteralBackend(View const &view) noexcept;

    [[nodiscard]] inline size_t hash() const noexcept { return hash_; }

    explicit operator View() const noexcept;
};

}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_FALLBACKLITERALBACKEND_HPP
