#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {


class IRIBackend {
    std::string iri;

public:
    explicit IRIBackend(std::string_view iri) noexcept;
    explicit IRIBackend(handle::IRIBackendView view) noexcept;
    auto operator<=>(const IRIBackend &) const = default;
    auto operator<=>(handle::IRIBackendView const &other) const noexcept {
        return handle::IRIBackendView(*this) <=> other;
    }
    std::partial_ordering operator<=>(std::unique_ptr<IRIBackend> const &other) const noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    explicit operator handle::IRIBackendView() const noexcept;
};
std::partial_ordering operator<=>(std::unique_ptr<IRIBackend> const &self, std::unique_ptr<IRIBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
namespace rdf4cpp::rdf::storage::node::handle {
inline std::partial_ordering operator<=>(IRIBackendView const &lhs, std::unique_ptr<reference_node_storage::IRIBackend> const &rhs) noexcept {
    return lhs <=> IRIBackendView(*rhs);
}
}  // namespace rdf4cpp::rdf::storage::node::handle
#endif  //RDF4CPP_IRIBACKEND_HPP
