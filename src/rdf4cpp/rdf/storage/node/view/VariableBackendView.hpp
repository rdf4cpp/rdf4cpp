#ifndef RDF4CPP_VARIABLEBACKENDHANDLE_HPP
#define RDF4CPP_VARIABLEBACKENDHANDLE_HPP

#include <compare>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::view {

struct VariableBackendView {
    std::string_view name;
    bool is_anonymous;

    /**
     * N-Triples conform string representation.
     * @return N-Triples conform string representation.
     */
    [[nodiscard]] std::string n_string() const noexcept;

    auto operator<=>(VariableBackendView const &) const noexcept = default;
};

}  // namespace rdf4cpp::rdf::storage::node::view
#endif  //RDF4CPP_VARIABLEBACKENDHANDLE_HPP
