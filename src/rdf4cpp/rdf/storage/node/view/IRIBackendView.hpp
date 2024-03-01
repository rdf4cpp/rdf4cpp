#ifndef RDF4CPP_IRIBACKENDHANDLE_HPP
#define RDF4CPP_IRIBACKENDHANDLE_HPP

#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::view {

struct IRIBackendView {
    std::string_view identifier;

    auto operator<=>(IRIBackendView const &) const noexcept = default;
    [[nodiscard]] size_t hash() const noexcept;
};
}  // namespace rdf4cpp::rdf::storage::node::view

template<>
struct std::hash<rdf4cpp::rdf::storage::node::view::IRIBackendView> {
    size_t operator()(rdf4cpp::rdf::storage::node::view::IRIBackendView const &x) const noexcept;
};

#endif  //RDF4CPP_IRIBACKENDHANDLE_HPP
