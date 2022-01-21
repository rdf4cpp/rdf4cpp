#ifndef RDF4CPP_BACKENDNODEHANDLES_HPP
#define RDF4CPP_BACKENDNODEHANDLES_HPP

#include <rdf4cpp/rdf/storage/node/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node {

struct BNodeBackendHandle {
    std::string_view identifier;
    [[nodiscard]] std::string n_string() const noexcept {
        return "_:" + std::string{identifier};
    }
    auto operator<=>( BNodeBackendHandle const&) const noexcept = default;
};

struct IRIBackendHandle {
    std::string_view identifier;

    [[nodiscard]] std::string n_string() const noexcept {
        return "<" + std::string{identifier} + ">";
    }

    auto operator<=>( IRIBackendHandle const&) const noexcept = default;
};

struct LiteralBackendHandle {
    NodeID datatype_id;
    std::string_view lexical_form;
    std::string_view language_tag;

    auto operator<=>( LiteralBackendHandle const&) const noexcept = default;
};

struct VariableBackendHandle {
    std::string_view name;
    bool is_anonymous;
    [[nodiscard]] std::string n_string() const noexcept {
        if (is_anonymous)
            return "_:" + std::string{name};
        else
            return "?" + std::string{name};
    }

    auto operator<=>( VariableBackendHandle const&) const noexcept = default;
};

}
#endif  //RDF4CPP_BACKENDNODEHANDLES_HPP
