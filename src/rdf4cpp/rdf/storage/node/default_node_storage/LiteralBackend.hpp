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
    LiteralBackend(std::string_view lexical, identifier::NodeID dataType, std::string_view langTag = "") noexcept;
    explicit LiteralBackend(handle::LiteralBackendView view) noexcept;
    std::partial_ordering operator<=>(LiteralBackend const &) const noexcept;
    auto operator<=>(handle::LiteralBackendView const &other) const noexcept {
        return handle::LiteralBackendView(*this) <=> other;
    }
    std::partial_ordering operator<=>(std::unique_ptr<LiteralBackend> const &other) const noexcept;

    bool operator==(const LiteralBackend &) const noexcept;

    [[nodiscard]] std::string_view lexical_form() const noexcept;

    [[nodiscard]] const identifier::NodeID &datatype_id() const noexcept;

    [[nodiscard]] std::string_view language_tag() const noexcept;

    explicit operator handle::LiteralBackendView() const noexcept;
};


std::partial_ordering operator<=>(std::unique_ptr<LiteralBackend> const &self, std::unique_ptr<LiteralBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage

namespace rdf4cpp::rdf::storage::node::handle {
inline std::partial_ordering operator<=>(LiteralBackendView const &lhs, std::unique_ptr<default_node_storage::LiteralBackend> const &rhs) noexcept {
    return lhs <=> LiteralBackendView(*rhs);
}
}  // namespace rdf4cpp::rdf::storage::node::handle

#endif  //RDF4CPP_LITERALBACKEND_HPP
