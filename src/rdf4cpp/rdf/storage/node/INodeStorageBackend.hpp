#ifndef RDF4CPP_INODESTORAGEBACKEND_HPP
#define RDF4CPP_INODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeStorageID.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>

#include <atomic>
#include <cstddef>
#include <functional>
#include <mutex>

namespace rdf4cpp::rdf::storage::node {

class NodeStorage;

/**
 * Interface that must be implemented by any NodeStorageBackendImplementation. A reference implementation is available with reference_node_storage::ReferenceNodeStorageBackend..
 */
class INodeStorageBackend {
    friend NodeStorage;
    static identifier::NodeStorageID register_node_context(INodeStorageBackend *);

public:
    using DependentAssetCleaner = std::function<void()>;

protected:
    /**
     * Mutex guarding construction and destruction of INodeStorageBackend instances.
     */
    static std::mutex destruction_mutex_;
    /**
     * Traces how many instances of NodeStorage use this INodeStorage.
     */
    std::atomic<size_t> use_count_ = 1;
    /**
     * Traces how many nodes this NodeStorage has.
     */
    std::atomic<size_t> nodes_in_use_ = 0;
    /**
     * The NodeStorageID of this. Using the constructor ensures that it is registered and unique.
     */
    identifier::NodeStorageID manager_id;

    std::vector<DependentAssetCleaner> dependent_asset_cleaners_;

    bool destruction_ongoing = false;

    /**
     * Increment the use count. Is called by NodeStorage constructor. Synchronized.
     */
    void inc_use_count() noexcept;
    /**
     * Decease the use count. Is called by NodeStorage destructor. If use_count() const and nodes_in_use() const are both 0 afterwards, this is destructed. Synchronized.
     */
    void dec_use_count() noexcept;
    /**
     * Increment the number of <div>Node</div>s tracked by this INodeStorage. Synchronized.
     */
    void inc_nodes_in_use() noexcept;
    /**
     * Decrements the number of <div>Node</div>s tracked by this INodeStorage. If use_count() const and nodes_in_use() const are both 0 afterwards, this is destructed. Synchronized.
     */
    void dec_nodes_in_use() noexcept;

protected:
    virtual ~INodeStorageBackend() = 0;

public:
    /**
     * Constructor. Synchronized.
     */
    INodeStorageBackend();

    /**
     * Number of NodeStorage instances using this INodeStorageBackend.
     * @return
     */
    [[nodiscard]] size_t use_count() const noexcept;

    /**
     * Number of <dev>Node</dev>s managed by this INodeStorageBackend.
     * @return
     */
    [[nodiscard]] size_t nodes_in_use() const noexcept;

    void register_dependent_asset_cleaner(DependentAssetCleaner dependent_asset_cleaner) noexcept;

    /**
      * Backend for NodeStorage::find_or_make_id(view::BNodeBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a BlankNode stored at the implementation of INodeStorageBackend.
      */
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(view::BNodeBackendView const &view) noexcept = 0;
    /**
      * Backend for NodeStorage::find_or_make_id(view::IRIBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a IRI stored at the implementation of INodeStorageBackend.
      */
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(view::IRIBackendView const &view) noexcept = 0;
    /**
      * Backend for NodeStorage::find_or_make_id(view::LiteralBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a BlankNode stored at the implementation of INodeStorageBackend.
      */
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(view::LiteralBackendView const &view) noexcept = 0;
    /**
      * Backend for NodeStorage::find_or_make_id(view::VariableBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a Variable stored at the implementation of INodeStorageBackend.
      */
    [[nodiscard]] virtual identifier::NodeID find_or_make_id(view::VariableBackendView const &view) noexcept = 0;

    /**
      * Backend for NodeStorage::find_id(view::BNodeBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a BlankNode stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such BlankNode stored.
      */
    [[nodiscard]] virtual identifier::NodeID find_id(view::BNodeBackendView const &view) const noexcept = 0;
    /**
      * Backend for NodeStorage::find_id(view::IRIBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a IRI stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such IRI stored.
      */
    [[nodiscard]] virtual identifier::NodeID find_id(view::IRIBackendView const &view) const noexcept = 0;
    /**
      * Backend for NodeStorage::find_id(view::LiteralBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a Literal stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such Literal stored.
      */
    [[nodiscard]] virtual identifier::NodeID find_id(view::LiteralBackendView const &view) const noexcept = 0;
    /**
      * Backend for NodeStorage::find_id(view::VariableBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a Variable stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such Variable stored.
      */
    [[nodiscard]] virtual identifier::NodeID find_id(view::VariableBackendView const &view) const noexcept = 0;

    /**
     * Backend for NodeStorage::find_iri_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::IRIBackendView describing the requested Node
     */
    [[nodiscard]] virtual view::IRIBackendView find_iri_backend_view(identifier::NodeID id) const = 0;
    /**
     * Backend for NodeStorage::find_literal_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::LiteralBackendView describing the requested Node
     */
    [[nodiscard]] virtual view::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const = 0;
    /**
     * Backend for NodeStorage::find_bnode_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::BNodeBackendView describing the requested Node
     */
    [[nodiscard]] virtual view::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const = 0;
    /**
     * Backend for NodeStorage::find_variable_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::VariableBackendView describing the requested Node
     */
    [[nodiscard]] virtual view::VariableBackendView find_variable_backend_view(identifier::NodeID id) const = 0;

    /**
     * Backend for NodeStorage::erase_iri(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_iri(identifier::NodeID id) const = 0;
    /**
     * Backend for NodeStorage::erase_literal(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_literal(identifier::NodeID id) const = 0;
    /**
     * Backend for NodeStorage::erase_bnode(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_bnode(identifier::NodeID id) const = 0;
    /**
     * Backend for NodeStorage::erase_variable(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    virtual bool erase_variable(identifier::NodeID id) const = 0;
};

}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_INODESTORAGEBACKEND_HPP
