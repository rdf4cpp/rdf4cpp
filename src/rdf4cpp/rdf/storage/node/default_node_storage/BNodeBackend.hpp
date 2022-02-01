#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::default_node_storage {

class BNodeBackend {
    std::string identifier_;

public:
    explicit BNodeBackend(std::string_view identifier) noexcept;
    explicit BNodeBackend(handle::BNodeBackendView view) noexcept;
    auto operator<=>(BNodeBackend const &) const noexcept = default;
    auto operator<=>(handle::BNodeBackendView const &other) const noexcept {
        return handle::BNodeBackendView(*this) <=> other;
    }
    std::partial_ordering operator<=>(std::unique_ptr<BNodeBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    explicit operator handle::BNodeBackendView() const noexcept;
};

std::partial_ordering operator<=>(std::unique_ptr<BNodeBackend> const &self, std::unique_ptr<BNodeBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage

namespace rdf4cpp::rdf::storage::node::handle {
inline std::partial_ordering operator<=>(BNodeBackendView const &lhs, std::unique_ptr<default_node_storage::BNodeBackend> const &rhs) noexcept {
    return lhs <=> BNodeBackendView(*rhs);
}
}  // namespace rdf4cpp::rdf::storage::node::handle
#endif  //RDF4CPP_BNODEBACKEND_HPP
