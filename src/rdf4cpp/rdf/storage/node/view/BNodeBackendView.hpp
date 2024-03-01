#ifndef RDF4CPP_BNODEBACKENDHANDLE_HPP
#define RDF4CPP_BNODEBACKENDHANDLE_HPP

#include <string_view>
#include <optional>

#include <rdf4cpp/rdf/bnode_mngt/WeakNodeScope.hpp>

namespace rdf4cpp::rdf::storage::node::view {

struct BNodeBackendView {
    std::string_view identifier;
    std::optional<rdf4cpp::rdf::bnode_mngt::WeakNodeScope> scope;

    [[nodiscard]] size_t hash() const noexcept;
    auto operator<=>(BNodeBackendView const &other) const noexcept = default;
};
}  // namespace rdf4cpp::rdf::storage::node::view

template<>
struct std::hash<rdf4cpp::rdf::storage::node::view::BNodeBackendView> {
    size_t operator()(rdf4cpp::rdf::storage::node::view::BNodeBackendView const &x) const noexcept;
};

#endif  //RDF4CPP_BNODEBACKENDHANDLE_HPP
