#ifndef RDF4CPP_PLACEHOLDERREFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_PLACEHOLDERREFERENCENODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct PlaceholderReferenceNodeStorageBackend : INodeStorageBackend {
    constexpr PlaceholderReferenceNodeStorageBackend() noexcept = default;

    [[nodiscard]] size_t size() const noexcept override {
        return 0;
    }
    [[nodiscard]] bool has_specialized_storage_for([[maybe_unused]] identifier::LiteralType datatype) const noexcept override {
        return false;
    }
    [[nodiscard]] identifier::NodeID find_or_make_id([[maybe_unused]] view::BNodeBackendView const &view) noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_or_make_id([[maybe_unused]] view::IRIBackendView const &view) noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_or_make_id([[maybe_unused]] view::LiteralBackendView const &view) noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_or_make_id([[maybe_unused]] view::VariableBackendView const &view) noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_id([[maybe_unused]] view::BNodeBackendView const &view) const noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_id([[maybe_unused]] view::IRIBackendView const &view) const noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_id([[maybe_unused]] view::LiteralBackendView const &view) const noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] identifier::NodeID find_id([[maybe_unused]] view::VariableBackendView const &view) const noexcept override {
        return identifier::NodeID{};
    }
    [[nodiscard]] view::IRIBackendView find_iri_backend_view([[maybe_unused]] identifier::NodeID id) const override {
        throw std::logic_error{"Forgot to populate placeholder NodeStorage"};
    }
    [[nodiscard]] view::LiteralBackendView find_literal_backend_view([[maybe_unused]] identifier::NodeID id) const override {
        throw std::logic_error{"Forgot to populate placeholder NodeStorage"};
    }
    [[nodiscard]] view::BNodeBackendView find_bnode_backend_view([[maybe_unused]] identifier::NodeID id) const override {
        throw std::logic_error{"Forgot to populate placeholder NodeStorage"};
    }
    [[nodiscard]] view::VariableBackendView find_variable_backend_view([[maybe_unused]] identifier::NodeID id) const override {
        throw std::logic_error{"Forgot to populate placeholder NodeStorage"};
    }
    bool erase_iri([[maybe_unused]] identifier::NodeID id) noexcept override {
        return false;
    }
    bool erase_literal([[maybe_unused]] identifier::NodeID id) noexcept override {
        return false;
    }
    bool erase_bnode([[maybe_unused]] identifier::NodeID id) noexcept override {
        return false;
    }
    bool erase_variable([[maybe_unused]] identifier::NodeID id) noexcept override {
        return false;
    }
};

} // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif // RDF4CPP_PLACEHOLDERREFERENCENODESTORAGEBACKEND_HPP
