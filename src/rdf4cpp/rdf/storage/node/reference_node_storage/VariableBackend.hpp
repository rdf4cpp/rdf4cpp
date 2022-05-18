#ifndef RDF4CPP_VARIABLEBACKEND_HPP
#define RDF4CPP_VARIABLEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

class VariableBackend {
    std::string name_;
    bool anonymous_;
    size_t hash_;

public:
    using View = view::VariableBackendView;
    explicit VariableBackend(std::string_view name, bool anonymous = false) noexcept;
    explicit VariableBackend(view::VariableBackendView view) noexcept;

    [[nodiscard]] bool is_anonymous() const noexcept;

    [[nodiscard]] std::string_view name() const noexcept;

    [[nodiscard]] size_t hash() const noexcept { return hash_; }

    explicit operator view::VariableBackendView() const noexcept;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_VARIABLEBACKEND_HPP
