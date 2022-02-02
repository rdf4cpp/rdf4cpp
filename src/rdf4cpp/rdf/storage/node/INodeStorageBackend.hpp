#ifndef RDF4CPP_INODESTORAGEBACKEND_HPP
#define RDF4CPP_INODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeStorageID.hpp>

#include <atomic>
#include <cstddef>

namespace rdf4cpp::rdf::storage::node {

class NodeStorage;

class INodeStorageBackend {
    friend NodeStorage;
    static identifier::NodeStorageID register_node_context(INodeStorageBackend *);

protected:
    // TODO: usage tracking can lead to race conditions
    std::atomic<size_t> use_count_ = 1;
    size_t nodes_in_use_ = 0;
    identifier::NodeStorageID manager_id;

    void inc_use_count() noexcept;
    void dec_use_count() noexcept;
    void inc_nodes_in_use() noexcept;
    void dec_nodes_in_use() noexcept;

    [[nodiscard]] bool is_unreferenced() const noexcept;

public:
    [[nodiscard]] size_t use_count() const noexcept;

    [[nodiscard]] size_t nodes_in_use() const noexcept;
    INodeStorageBackend();
    virtual ~INodeStorageBackend() = 0;

    [[nodiscard]] virtual identifier::NodeID find_or_make_id(handle::BNodeBackendView const &) noexcept = 0;
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(handle::IRIBackendView const &) noexcept = 0;
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(handle::LiteralBackendView const &) noexcept = 0;
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(handle::VariableBackendView const &) noexcept = 0;

    [[nodiscard]] virtual identifier::NodeID find_id(handle::BNodeBackendView const &) const noexcept = 0;
    [[nodiscard]] virtual identifier::NodeID find_id(handle::IRIBackendView const &) const noexcept = 0;
    [[nodiscard]] virtual identifier::NodeID find_id(handle::LiteralBackendView const &) const noexcept = 0;
    [[nodiscard]] virtual identifier::NodeID find_id(handle::VariableBackendView const &) const noexcept = 0;

    [[nodiscard]] virtual handle::IRIBackendView find_iri_backend_view(identifier::NodeID id) const = 0;
    [[nodiscard]] virtual handle::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const = 0;
    [[nodiscard]] virtual handle::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const = 0;
    [[nodiscard]] virtual handle::VariableBackendView find_variable_backend_view(identifier::NodeID id) const = 0;

    /**
     * Erase an IRI's backend. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_iri(identifier::NodeID id) const = 0;
    /**
     * Erase a Literal's backend. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_literal(identifier::NodeID id) const = 0;
    /**
     * Erase a BlankNode's backend. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_bnode(identifier::NodeID id) const = 0;
    /**
     * Erase a Variable's backend. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_variable(identifier::NodeID id) const = 0;
};

}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_INODESTORAGEBACKEND_HPP
