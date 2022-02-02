#ifndef RDF4CPP_NODESTORAGE_HPP
#define RDF4CPP_NODESTORAGE_HPP

#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/ReferenceNodeStorageBackend.hpp>

#include <memory>
#include <mutex>
#include <optional>

namespace rdf4cpp::rdf::storage::node {

class INodeStorageBackend;

/**
 * NodeStorage provides an interface to the internal storage <div>Node</div>s.
 * Each NodeStorage has a INodeStorageBackend, which is uniquely identified by a identifier::NodeStorageID.
 * There can be at most 1024 different INodeStorageBackend instances at once.
 * If no NodeStorage with the identifier::NodeStorageID of a certain INodeStorageBackend exists anymore, the corresponding INodeStorageBackend is destructed.
 * This does not apply to the default_instance.
 * The live cycle of NodeStorage's and their backends is managed by the static methods within this class.
 */
class NodeStorage {
    friend INodeStorageBackend;

    /*
     * Static fields and methods for life cycle management.
     */

    /**
     * Static array storing up to 1024 node_context Instances. As key identifier::NodeStorageID is used.
     */
    static inline std::array<INodeStorageBackend *, 1024> node_context_instances{};
    /**
     * Makes sure the default_instance_ and default_node_context_id are initialized only once
     */
    static inline std::once_flag default_init_once_flag;
    /*
     * identifier::NodeStorageID of the current default instance
     */
    static inline identifier::NodeStorageID default_node_context_id;
    /**
     * THe default instance used whenever no NodeStorage is explicitly provided.
     */
    static NodeStorage default_instance_;
    /**
     * Retrieve a backend_instance from node_context_instances by its ID
     * @param id ID of the INodeStorage
     * @return pointer to the instance or null pointer if no instance is registered for the given ID
     */
    static INodeStorageBackend *lookup_backend_instance(identifier::NodeStorageID id);

public:
    /**
     * Get the default NodeStorage instance. This is the instance that is used wherever no NodeStorage is explicitly specified.
     * @return
     */
    static NodeStorage &default_instance();

    /**
     * Change the default instance.
     * @param node_context NodeStorage that becomes default NodeStorage.
     */
    static void default_instance(const NodeStorage &node_context);

    /**
     * Create a NodeStorage with a custom Backend.
     * @tparam BackendImpl Class deriving from INodeStorage
     * @tparam Args types of args
     * @param args arguments to construct BackendImpl
     * @return NodeStorage encapsulating the instance.
     */
    template<typename BackendImpl, typename... Args>
    static inline NodeStorage new_instance(Args... args) requires std::is_base_of<INodeStorageBackend, BackendImpl>::value {
        return NodeStorage(new BackendImpl(args...));
    }

    /**
     * Create a new instance of reference_node_storage::ReferenceNodeStorageBackend.
     * @return NodeStorage backed by reference_node_storage::ReferenceNodeStorageBackend
     */
    static NodeStorage new_instance();

    /**
     * Retrieve NodeStorage for the given ID. If no such NodeStorage exists, std::nullopt is returned.
     * @param id NodeStorage id
     * @return optional NodeStorage
     */
    static std::optional<NodeStorage> lookup_instance(identifier::NodeStorageID id);

    /**
     * <p>Use with caution!</p>
     * <p>Registers a INodeStorageBackend at the identifier::NodeStorageID provided by the instance.
     * Will throw if either a null pointer is provided or the identifier::NodeStorageID is already taken.
     * To ensure that NodeStorage reference counting works as expected, INodeStorageBackend::use_count_ must be 1.</p>
     * @param backend_instance instance to be registered
     * @return an NodeStorage encapsulating backend_instance
     */
    static NodeStorage register_backend(INodeStorageBackend *backend_instance);

    /**
     * <p>Use with caution!</p>
     * <p>Unregisters the InodeStorageBackend at the identifier::NodeStorageID provided in backend_instance.
     * This method should only be used if the backend_instance was registered with NodeStorage::register_backend and had an initial use_count_ of >1.
     * At the time of calling this method, backend_instance must not be the default_instance and there must be no NodeStorage instances using backend_instance.</p>
     * @param backend_instance
     */
    static void unregister_backend(INodeStorageBackend *backend_instance);

    /*
     * Fields
     */

    /**
     * Backend instance which provides the actual implementation of the methods.
     */
private:
    INodeStorageBackend *backend_{};

    /*
     * Constructors & Destructor
     */

    /**
     * Default construction is private. Use NodeStorage::default_instance or NodeStorage::new_instance.
     */
    NodeStorage() = default;

    /**
     * Constructs a NodeStorage with the given backend instance.
     * @param backend backend instance.
     */
    explicit NodeStorage(INodeStorageBackend *backend) noexcept : backend_(backend) {}

public:
    NodeStorage(NodeStorage &&other) noexcept;
    NodeStorage(const NodeStorage &node_context) noexcept;
    NodeStorage &operator=(const NodeStorage &other) noexcept;
    NodeStorage &operator=(NodeStorage &&other) noexcept;

    ~NodeStorage();

    /*
     * Member functions
     */

    /**
     * NodeStorage does instance counting. If no instances if a NodeStorage exist anymore its backend_instance_ is destructed.
     * For the default_instance an additional instance is kept so that its not even then destructed if the there are no application held instance left.
     * @return current number of instances of this NodeStorage
     */
    [[nodiscard]] size_t use_count() const noexcept;

    /**
     * Number of nodes currently held by this node storage.
     * @return
     */
    [[nodiscard]] size_t nodes_in_use() const noexcept;

    /**
     * Identifier of this NodeStorage
     * @return
     */
    [[nodiscard]] identifier::NodeStorageID id() const noexcept;


    /**
     * Lookup the identifier::NodeID for the given handle::BNodeBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view BlankNode description (MUST be valid)
     * @return identifier::NodeID identifying the requested BlankNode.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::BNodeBackendView const &view) noexcept;
    /**
     * Lookup the identifier::NodeID for the given handle::IRIBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view IRI description (MUST be valid)
     * @return identifier::NodeID identifying the requested IRI.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::IRIBackendView const &view) noexcept;
    /**
     * Lookup the identifier::NodeID for the given handle::LiteralBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view Literal description (MUST be valid)
     * @return identifier::NodeID identifying the requested Literal.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::LiteralBackendView const &view) noexcept;
    /**
     * Lookup the identifier::NodeID for the given handle::VariableBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view Variable description (MUST be valid)
     * @return identifier::NodeID identifying the requested Variable.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::VariableBackendView const &view) noexcept;

    /**
     * Lookup the identifier::NodeID for the given handle::BNodeBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view BlankNode description (MUST be valid)
     * @return identifier::NodeID identifying the requested BlankNode.
     */
    [[nodiscard]] identifier::NodeID find_id(handle::BNodeBackendView const &view) const noexcept;
    /**
     * Lookup the identifier::NodeID for the given handle::IRIBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view IRI description (MUST be valid)
     * @return identifier::NodeID identifying the requested IRI.
     */
    [[nodiscard]] identifier::NodeID find_id(handle::IRIBackendView const &view) const noexcept;
    /**
     * Lookup the identifier::NodeID for the given handle::LiteralBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view Literal description (MUST be valid)
     * @return identifier::NodeID identifying the requested Literal.
     */
    [[nodiscard]] identifier::NodeID find_id(handle::LiteralBackendView const &view) const noexcept;
    /**
     * Lookup the identifier::NodeID for the given handle::VariableBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view Variable description (MUST be valid)
     * @return identifier::NodeID identifying the requested Variable.
     */
    [[nodiscard]] identifier::NodeID find_id(handle::VariableBackendView const &view) const noexcept;

    /**
     * Lookup the handle::IRIBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return handle::IRIBackendView describing the requested resource.
     */
    [[nodiscard]] handle::IRIBackendView find_iri_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the handle::LiteralBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return handle::LiteralBackendView describing the requested resource.
     */
    [[nodiscard]] handle::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the handle::BNodeBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return handle::BNodeBackendView describing the requested resource.
     */
    [[nodiscard]] handle::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the handle::VariableBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return handle::VariableBackendView describing the requested resource.
     */
    [[nodiscard]] handle::VariableBackendView find_variable_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the handle::IRIBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or handle.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return handle::IRIBackendView describing the requested resource.
     */
    [[nodiscard]] static handle::IRIBackendView find_iri_backend_view(identifier::NodeBackendHandle handle);
    /**
     * Lookup the handle::LiteralBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or handle.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return handle::LiteralBackendView describing the requested resource.
     */
    [[nodiscard]] static handle::LiteralBackendView find_literal_backend_view(identifier::NodeBackendHandle handle);
    /**
     * Lookup the handle::BNodeBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or handle.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return handle::BNodeBackendView describing the requested resource.
     */
    [[nodiscard]] static handle::BNodeBackendView find_bnode_backend_view(identifier::NodeBackendHandle handle);
    /**
     * Lookup the handle::VariableBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or handle.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return handle::VariableBackendView describing the requested resource.
     */
    [[nodiscard]] static handle::VariableBackendView find_variable_backend_view(identifier::NodeBackendHandle handle);

    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.
     * This includes also Literals which use this IRI as datatype.  </p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_iri(identifier::NodeID id) const;
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_literal(identifier::NodeID id) const;
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_bnode(identifier::NodeID id) const;
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_variable(identifier::NodeID id) const;
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.
     * This includes also Literals which use this IRI as datatype.  </p>
     * <p>If handle.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_iri(identifier::NodeBackendHandle handle);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.
     * <p>If handle.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_literal(identifier::NodeBackendHandle handle);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>If handle.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_bnode(identifier::NodeBackendHandle handle);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>If handle.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_variable(identifier::NodeBackendHandle handle);

    bool operator==(const NodeStorage &other) const noexcept = default;
};


}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_NODESTORAGE_HPP
