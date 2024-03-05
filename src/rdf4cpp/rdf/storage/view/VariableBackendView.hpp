#ifndef RDF4CPP_VARIABLEBACKENDHANDLE_HPP
#define RDF4CPP_VARIABLEBACKENDHANDLE_HPP

#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::view {

struct VariableBackendView {
    std::string_view name;
    bool is_anonymous;

    auto operator<=>(VariableBackendView const &) const noexcept = default;
    [[nodiscard]] size_t hash() const noexcept;
};

}  // namespace rdf4cpp::rdf::storage::view

template<>
struct std::hash<rdf4cpp::rdf::storage::view::VariableBackendView> {
    size_t operator()(rdf4cpp::rdf::storage::view::VariableBackendView const &x) const noexcept;
};

#endif  //RDF4CPP_VARIABLEBACKENDHANDLE_HPP
