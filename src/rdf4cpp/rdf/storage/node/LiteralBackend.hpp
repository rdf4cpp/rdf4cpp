#ifndef RDF4CPP_LITERALBACKEND_HPP
#define RDF4CPP_LITERALBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node {

struct LiteralBackendHandle {
    NodeID datatype_id;
    std::string_view lexical_form;
    std::string_view language_tag;

    auto operator<=>( LiteralBackendHandle const&) const noexcept = default;
};

class LiteralBackend {
    NodeID datatype_id_;
    std::string lexical;
    std::string lang_tag;

public:
    [[nodiscard]] std::string quote_lexical() const noexcept;
    LiteralBackend(std::string_view lexical, const NodeID &dataType, std::string_view langTag = "") noexcept;
    std::strong_ordering operator<=>(const LiteralBackend &) const noexcept;
    std::strong_ordering operator<=>(std::unique_ptr<LiteralBackend> const &other) const noexcept;

    bool operator==(const LiteralBackend &) const noexcept;

    [[nodiscard]] std::string_view lexical_form() const noexcept;

    [[nodiscard]] const NodeID &datatype_id() const noexcept;

    [[nodiscard]] std::string_view language_tag() const noexcept;

    explicit operator LiteralBackendHandle() const noexcept {
        return {.datatype_id = datatype_id(),
                .lexical_form = lexical_form(),
                .language_tag = language_tag()};
    }
};


std::strong_ordering operator<=>(std::unique_ptr<LiteralBackend> const &self, std::unique_ptr<LiteralBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_LITERALBACKEND_HPP
