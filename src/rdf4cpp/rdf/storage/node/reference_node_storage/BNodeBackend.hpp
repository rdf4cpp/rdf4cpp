#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

class BNodeBackend {
    std::string identifier_;
    size_t hash_;

public:
    using View = view::BNodeBackendView;
    explicit BNodeBackend(std::string_view identifier) noexcept;
    explicit BNodeBackend(view::BNodeBackendView view) noexcept;
    auto operator<=>(BNodeBackend const &) const noexcept = default;
    auto operator<=>(view::BNodeBackendView const &other) const noexcept {
        return view::BNodeBackendView(*this) <=> other;
    }
    std::partial_ordering operator<=>(std::unique_ptr<BNodeBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    [[nodiscard]] size_t hash() const noexcept { return hash_; }

    explicit operator view::BNodeBackendView() const noexcept;
};

std::partial_ordering operator<=>(std::unique_ptr<BNodeBackend> const &self, std::unique_ptr<BNodeBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

template<>
struct std::hash<rdf4cpp::rdf::storage::node::reference_node_storage::BNodeBackend> {
    size_t operator()(rdf4cpp::rdf::storage::node::reference_node_storage::BNodeBackend const &x) const noexcept {
        return x.hash();
    }
};

namespace rdf4cpp::rdf::storage::node::view {
inline std::partial_ordering operator<=>(BNodeBackendView const &lhs, std::unique_ptr<reference_node_storage::BNodeBackend> const &rhs) noexcept {
    return lhs <=> BNodeBackendView(*rhs);
}
}  // namespace rdf4cpp::rdf::storage::node::view
#endif  //RDF4CPP_BNODEBACKEND_HPP
