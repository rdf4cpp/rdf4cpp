#ifndef RDF4CPP_BNODEBACKENDHANDLE_HPP
#define RDF4CPP_BNODEBACKENDHANDLE_HPP

#include <string>
#include <string_view>

namespace rdf4cpp::rdf::bnode_mngt {
struct WeakNodeScope;
} // namespace rdf4cpp::rdf::bnode_mngt

namespace rdf4cpp::rdf::storage::node::view {

struct BNodeBackendView {
    std::string_view identifier;
    rdf4cpp::rdf::bnode_mngt::WeakNodeScope const *scope;

    auto operator<=>(BNodeBackendView const &) const noexcept = default;
    [[nodiscard]] size_t hash() const noexcept;
};
}  // namespace rdf4cpp::rdf::storage::node::view

template<>
struct std::hash<rdf4cpp::rdf::storage::node::view::BNodeBackendView> {
    size_t operator()(rdf4cpp::rdf::storage::node::view::BNodeBackendView const &x) const noexcept;
};

#endif  //RDF4CPP_BNODEBACKENDHANDLE_HPP
