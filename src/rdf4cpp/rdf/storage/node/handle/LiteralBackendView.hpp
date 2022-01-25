#ifndef RDF4CPP_LITERALBACKENDHANDLE_HPP
#define RDF4CPP_LITERALBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::handle {

struct LiteralBackendView {
    identifier::NodeID datatype_id;
    std::string_view lexical_form;
    std::string_view language_tag;

    auto operator<=>(LiteralBackendView const &) const noexcept = default;
};
}  // namespace rdf4cpp::rdf::storage::node::handle

#endif  //RDF4CPP_LITERALBACKENDHANDLE_HPP
