#ifndef RDF4CPP_LITERALBACKEND_HPP
#define RDF4CPP_LITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::default_node_storage {

class LiteralBackend {
    identifier::NodeID datatype_id_;
    std::string lexical;
    std::string lang_tag;

public:
    [[nodiscard]] std::string quote_lexical() const noexcept;
    LiteralBackend(std::string_view lexical, const identifier::NodeID &dataType, std::string_view langTag = "") noexcept;
    std::strong_ordering operator<=>(const LiteralBackend &) const noexcept;
    std::strong_ordering operator<=>(std::unique_ptr<LiteralBackend> const &other) const noexcept;

    bool operator==(const LiteralBackend &) const noexcept;

    [[nodiscard]] std::string_view lexical_form() const noexcept;

    [[nodiscard]] const identifier::NodeID &datatype_id() const noexcept;

    [[nodiscard]] std::string_view language_tag() const noexcept;

    explicit operator handle::LiteralBackendView() const noexcept;
};


std::strong_ordering operator<=>(std::unique_ptr<LiteralBackend> const &self, std::unique_ptr<LiteralBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage

#endif  //RDF4CPP_LITERALBACKEND_HPP
