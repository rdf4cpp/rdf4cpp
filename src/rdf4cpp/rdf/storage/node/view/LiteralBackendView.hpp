#ifndef RDF4CPP_LITERALBACKENDHANDLE_HPP
#define RDF4CPP_LITERALBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::view {

struct LiteralBackendView {
    identifier::NodeID datatype_id;
    std::string_view lexical_form;
    std::string_view language_tag;

    auto operator<=>(LiteralBackendView const &) const noexcept = default;
    bool operator==(LiteralBackendView const &) const noexcept = default;

    [[nodiscard]] size_t hash() const noexcept;
};
}  // namespace rdf4cpp::rdf::storage::node::view

template<>
struct std::hash<rdf4cpp::rdf::storage::node::view::LiteralBackendView> {
    size_t operator()(rdf4cpp::rdf::storage::node::view::LiteralBackendView const &x) const noexcept;
};
#endif  //RDF4CPP_LITERALBACKENDHANDLE_HPP
