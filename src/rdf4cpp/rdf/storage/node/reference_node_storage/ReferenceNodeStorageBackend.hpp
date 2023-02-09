#ifndef RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
#define RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

#include <rdf4cpp/rdf/storage/node/reference_node_storage/NodeTypeStorage.hpp>

#include <rdf4cpp/rdf/storage/node/reference_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/IRIBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/VariableBackend.hpp>


namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * Thread-safe reference implementation of a INodeStorageBackend.
 */
class ReferenceNodeStorageBackend : public INodeStorageBackend {
public:
    using NodeID = identifier::NodeID;
    using LiteralID = identifier::LiteralID;

private:
    NodeTypeStorage<BNodeBackend> bnode_storage_;
    NodeTypeStorage<IRIBackend> iri_storage_;
    NodeTypeStorage<LiteralBackend> literal_storage_;
    NodeTypeStorage<VariableBackend> variable_storage_;

    LiteralID next_literal_id = NodeID::min_literal_id;
    NodeID next_bnode_id = NodeID::min_bnode_id;
    NodeID next_iri_id = NodeID::min_iri_id;
    NodeID next_variable_id = NodeID::min_variable_id;

public:
    ReferenceNodeStorageBackend() noexcept;

    [[nodiscard]] size_t size() const noexcept override;

    [[nodiscard]] identifier::NodeID find_or_make_id(view::BNodeBackendView const &) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::IRIBackendView const &) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::LiteralBackendView const &) noexcept override;
    [[nodiscard]] identifier::NodeID find_or_make_id(view::VariableBackendView const &) noexcept override;

    [[nodiscard]] identifier::NodeID find_id(view::BNodeBackendView const &) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(view::IRIBackendView const &) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(view::LiteralBackendView const &) const noexcept override;
    [[nodiscard]] identifier::NodeID find_id(view::VariableBackendView const &) const noexcept override;

    [[nodiscard]] view::IRIBackendView find_iri_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] view::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] view::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const override;
    [[nodiscard]] view::VariableBackendView find_variable_backend_view(identifier::NodeID id) const override;

    bool erase_iri(identifier::NodeID id) override;
    bool erase_literal(identifier::NodeID id) override;
    bool erase_bnode(identifier::NodeID id) override;
    bool erase_variable(identifier::NodeID id) override;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
#endif  //RDF4CPP_REFERENCENODESTORAGEBACKEND_HPP
