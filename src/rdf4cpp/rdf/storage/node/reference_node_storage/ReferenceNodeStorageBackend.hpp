#ifndef RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

#include <rdf4cpp/rdf/storage/node/reference_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/IRIBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/VariableBackend.hpp>

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * Thread-safe reference implementation of a INodeStorageBackend. Deleting nodes is not supported.
 */
class ReferenceNodeStorageBackend : public INodeStorageBackend {
public:
    using NodeID = identifier::NodeID;
    using LiteralID = identifier::LiteralID;

private:
    mutable std::shared_mutex literal_mutex_;
    std::map<NodeID, LiteralBackend *, std::less<>> literal_storage;
    std::map<std::unique_ptr<LiteralBackend>, NodeID, std::less<>> literal_storage_reverse;
    mutable std::shared_mutex bnode_mutex_;
    std::map<NodeID, BNodeBackend *, std::less<>> bnode_storage;
    std::map<std::unique_ptr<BNodeBackend>, NodeID, std::less<>> bnode_storage_reverse;
    mutable std::shared_mutex iri_mutex_;
    std::map<NodeID, IRIBackend *, std::less<>> iri_storage;
    std::map<std::unique_ptr<IRIBackend>, NodeID, std::less<>> iri_storage_reverse;
    mutable std::shared_mutex variable_mutex_;
    std::map<NodeID, VariableBackend *, std::less<>> variable_storage;
    std::map<std::unique_ptr<VariableBackend>, NodeID, std::less<>> variable_storage_reverse;

    LiteralID next_literal_id = NodeID::min_literal_id;
    NodeID next_bnode_id = NodeID::min_bnode_id;
    NodeID next_iri_id = NodeID::min_iri_id;
    NodeID next_variable_id = NodeID::min_variable_id;

public:
    ReferenceNodeStorageBackend();

    ~ReferenceNodeStorageBackend() override = default;

    [[nodiscard]] identifier::NodeID find_or_make_id(handle::BNodeBackendView const &) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::IRIBackendView const &) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::LiteralBackendView const &) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::VariableBackendView const &) noexcept override;

    [[nodiscard]] identifier::NodeID find_id(handle::BNodeBackendView const &) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(handle::IRIBackendView const &) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(handle::LiteralBackendView const &) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(handle::VariableBackendView const &) const noexcept override;

    [[nodiscard]] handle::IRIBackendView find_iri_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] handle::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] handle::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] handle::VariableBackendView find_variable_backend_view(identifier::NodeID id) const override;

    bool erase_iri(identifier::NodeID id) const override;
    bool erase_literal(identifier::NodeID id) const override;
    bool erase_bnode(identifier::NodeID id) const override;
    bool erase_variable(identifier::NodeID id) const override;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
#endif  //RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
