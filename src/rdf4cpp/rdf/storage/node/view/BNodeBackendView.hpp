#ifndef RDF4CPP_BNODEBACKENDHANDLE_HPP
#define RDF4CPP_BNODEBACKENDHANDLE_HPP

#include <compare>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::view {

struct BNodeBackendView {
    std::string_view identifier;

    /**
     * N-Triples conform string representation.
     * @return N-Triples conform string representation.
     */
    [[nodiscard]] std::string n_string() const noexcept;
    auto operator<=>(BNodeBackendView const &) const noexcept = default;
};
}  // namespace rdf4cpp::rdf::storage::node::view

#endif  //RDF4CPP_BNODEBACKENDHANDLE_HPP
