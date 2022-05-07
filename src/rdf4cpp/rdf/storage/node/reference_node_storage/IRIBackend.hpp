#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {


class IRIBackend {
    std::string iri;
    size_t hash_;

public:
    using View = view::IRIBackendView;
    explicit IRIBackend(std::string_view iri) noexcept;
    explicit IRIBackend(view::IRIBackendView view) noexcept;
    auto operator<=>(const IRIBackend &) const = default;
    auto operator<=>(view::IRIBackendView const &other) const noexcept {
        return view::IRIBackendView(*this) <=> other;
    }
    std::partial_ordering operator<=>(std::unique_ptr<IRIBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    explicit operator view::IRIBackendView() const noexcept;

    [[nodiscard]] size_t hash() const noexcept { return hash_; }
};
std::partial_ordering operator<=>(std::unique_ptr<IRIBackend> const &self, std::unique_ptr<IRIBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

template<>
struct std::hash<rdf4cpp::rdf::storage::node::reference_node_storage::IRIBackend> {
    size_t operator()(rdf4cpp::rdf::storage::node::reference_node_storage::IRIBackend const &x) const noexcept {
        return x.hash();
    }
};

namespace rdf4cpp::rdf::storage::node::view {
inline std::partial_ordering operator<=>(IRIBackendView const &lhs, std::unique_ptr<reference_node_storage::IRIBackend> const &rhs) noexcept {
    return lhs <=> IRIBackendView(*rhs);
}
}  // namespace rdf4cpp::rdf::storage::node::view
#endif  //RDF4CPP_IRIBACKEND_HPP
