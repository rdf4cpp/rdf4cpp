#ifndef RDF4CPP_BNODEBACKENDHANDLE_HPP
#define RDF4CPP_BNODEBACKENDHANDLE_HPP

#include <string_view>
#include <optional>

namespace rdf4cpp::storage::view {

struct BNodeBackendView {
    std::string_view identifier;

    [[nodiscard]] size_t hash() const noexcept;
    auto operator<=>(BNodeBackendView const &other) const noexcept = default;
};
}  // namespace rdf4cpp::storage::view

template<>
struct std::hash<rdf4cpp::storage::view::BNodeBackendView> {
    size_t operator()(rdf4cpp::storage::view::BNodeBackendView const &x) const noexcept;
};

#endif  //RDF4CPP_BNODEBACKENDHANDLE_HPP
