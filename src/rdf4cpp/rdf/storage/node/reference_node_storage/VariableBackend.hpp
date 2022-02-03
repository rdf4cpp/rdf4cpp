#ifndef RDF4CPP_VARIABLEBACKEND_HPP
#define RDF4CPP_VARIABLEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

class VariableBackend {
    std::string name_;
    bool anonymous_;

public:
    explicit VariableBackend(std::string_view name, bool anonymous = false) noexcept;
    explicit VariableBackend(handle::VariableBackendView view) noexcept;
    auto operator<=>(const VariableBackend &) const noexcept = default;
    auto operator<=>(handle::VariableBackendView const &other) const noexcept {
        return handle::VariableBackendView(*this) <=> other;
    }
    std::partial_ordering operator<=>(std::unique_ptr<VariableBackend> const &other) const noexcept;

    [[nodiscard]] bool is_anonymous() const noexcept;

    [[nodiscard]] std::string_view name() const noexcept;

    explicit operator handle::VariableBackendView() const noexcept;
};

std::partial_ordering operator<=>(std::unique_ptr<VariableBackend> const &self, std::unique_ptr<VariableBackend> const &other) noexcept;
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

namespace rdf4cpp::rdf::storage::node::handle {
inline std::partial_ordering operator<=>(VariableBackendView const &lhs, std::unique_ptr<reference_node_storage::VariableBackend> const &rhs) noexcept {
    return lhs <=> VariableBackendView(*rhs);
}
}  // namespace rdf4cpp::rdf::storage::node::handle
#endif  //RDF4CPP_VARIABLEBACKEND_HPP