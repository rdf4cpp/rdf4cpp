#ifndef RDF4CPP_NODESTORAGE_HPP
#define RDF4CPP_NODESTORAGE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/ReferenceNodeStorageBackend.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

#include <array>
#include <concepts>
#include <memory>
#include <mutex>
#include <optional>

namespace rdf4cpp::rdf::storage::node {

class INodeStorageBackend;
struct WeakNodeStorage;

namespace node_storage_detail {
static constexpr identifier::NodeStorageID INVALID_BACKEND_INDEX{static_cast<uint16_t>(-1)};

struct ControlBlock {
    std::atomic<INodeStorageBackend *> backend{nullptr};
    std::atomic<size_t> refcount{0};
    std::atomic<size_t> generation{0};
};
}  //namespace node_storage_detail

/**
 * NodeStorage provides an interface to the internal storage <div>Node</div>s.
 * Each NodeStorage has a INodeStorageBackend, which is uniquely identified by a identifier::NodeStorageID.
 * There can be at most 1024 different INodeStorageBackend instances at once.
 * If no NodeStorage with the identifier::NodeStorageID of a certain INodeStorageBackend exists anymore, the corresponding INodeStorageBackend is destructed.
 * This does not apply to the default_instance.
 * The lifecycle of NodeStorage's and their backends is managed by the static methods within this class.
 */
class NodeStorage {
    /**
     * An instance of this type essentially behaves like a shared_ptr
     * with a finite amount of fixed locations where it
     * can store its records (NodeStorage::node_context_instances).
     * These locations are also reused after the managed instance (in this case instances of INodeStorageBackend)
     * is destroyed.
     * This has a few implications for the design:
     *
     * 1. multiple threads trying to allocate a new backend will race to reserve a slot for themselves
     * 2. multiple threads that own a NodeStorage pointing to the same backend will race to change the reference count
     * 2.1. related: threads that own a NodeStorage pointing to the same backend will race to destroy it
     * 3. Weak node storages will race with node storages trying to destroy the backend when trying to upgrade them
     * 4. Threads owning a NodeStorage will race with threads owning a WeakNodeStorage on writing/reading the generation
     */

    friend struct WeakNodeStorage;

    /**
     * Static array storing up to 2^10 - 1 = 1023 node_context Instances. As key identifier::NodeStorageID is used.
     * The last value of a identifier::NodeStorageID (i.e. 1023) is reserved as the invalid index.
     */
    static inline std::array<node_storage_detail::ControlBlock, 1023> node_context_instances{};
    /**
     * Makes sure the default_instance_ and default_node_context_id are initialized only once
     */
    static inline std::once_flag default_init_once_flag;
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
    static void default_instance(NodeStorage const &node_context);

    /**
     * Create a NodeStorage with a custom Backend.
     * @tparam BackendImpl Class deriving from INodeStorage
     * @tparam Args types of args
     * @param args arguments to construct BackendImpl
     * @return NodeStorage encapsulating the instance.
     */
    template<typename BackendImpl, typename... Args>
    static inline NodeStorage new_instance(Args... args) requires std::derived_from<BackendImpl, INodeStorageBackend> {
        return register_backend(new BackendImpl(args...));
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
     * <p>Registers a INodeStorageBackend at the identifier::NodeStorageID provided by the instance.
     * Will throw if either a null pointer is provided.
     *
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

private:
    identifier::NodeStorageID backend_index = node_storage_detail::INVALID_BACKEND_INDEX;

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
    explicit NodeStorage(identifier::NodeStorageID backend_index) noexcept;

    [[nodiscard]] inline static node_storage_detail::ControlBlock &get_slot(identifier::NodeStorageID id) {
        assert(id != node_storage_detail::INVALID_BACKEND_INDEX);
        return NodeStorage::node_context_instances[static_cast<size_t>(id.value)];
    }

    [[nodiscard]] inline INodeStorageBackend &get_backend() noexcept {
        // SAFETY: this object is keeping the backend alive therefore the pointer must point to a valid backend instance
        return *get_slot(this->backend_index).backend.load(std::memory_order_relaxed);
    }

    [[nodiscard]] INodeStorageBackend const &get_backend() const noexcept {
        // SAFETY: this object is keeping the backend alive therefore the pointer must point to a valid backend instance
        return *get_slot(this->backend_index).backend.load(std::memory_order_relaxed);
    }

    void increase_refcount() noexcept;
    void decrease_refcount() noexcept;

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
    [[nodiscard]] size_t ref_count() const noexcept;

    /**
     * Number of nodes currently allocated by this node storage.
     * @return
     */
    [[nodiscard]] size_t size() const noexcept;

    /**
     * Identifier of this NodeStorage
     * @return
     */
    [[nodiscard]] identifier::NodeStorageID id() const noexcept;

    /**
     * Creates a WeakNodeStorage pointing to the same backend as this NodeStorage
     */
    [[nodiscard]] WeakNodeStorage downgrade() const noexcept;

    /**
     * Lookup the identifier::NodeID for the given view::BNodeBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view BlankNode description (MUST be valid)
     * @return identifier::NodeID identifying the requested BlankNode.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(view::BNodeBackendView const &view) noexcept;
    /**
     * Lookup the identifier::NodeID for the given view::IRIBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view IRI description (MUST be valid)
     * @return identifier::NodeID identifying the requested IRI.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(view::IRIBackendView const &view) noexcept;
    /**
     * Lookup the identifier::NodeID for the given view::LiteralBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view Literal description (MUST be valid)
     * @return identifier::NodeID identifying the requested Literal.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(view::LiteralBackendView const &view) noexcept;
    /**
     * Lookup the identifier::NodeID for the given view::VariableBackendView. If it doesn't exist in the backend yet, it is added.
     * @param view Variable description (MUST be valid)
     * @return identifier::NodeID identifying the requested Variable.
     */
    [[nodiscard]] identifier::NodeID find_or_make_id(view::VariableBackendView const &view) noexcept;

    /**
     * Lookup the identifier::NodeID for the given view::BNodeBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view BlankNode description (MUST be valid)
     * @return identifier::NodeID identifying the requested BlankNode.
     */
    [[nodiscard]] identifier::NodeID find_id(view::BNodeBackendView const &view) const noexcept;
    /**
     * Lookup the identifier::NodeID for the given view::IRIBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view IRI description (MUST be valid)
     * @return identifier::NodeID identifying the requested IRI.
     */
    [[nodiscard]] identifier::NodeID find_id(view::IRIBackendView const &view) const noexcept;
    /**
     * Lookup the identifier::NodeID for the given view::LiteralBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view Literal description (MUST be valid)
     * @return identifier::NodeID identifying the requested Literal.
     */
    [[nodiscard]] identifier::NodeID find_id(view::LiteralBackendView const &view) const noexcept;
    /**
     * Lookup the identifier::NodeID for the given view::VariableBackendView. If it doesn't exist, the method will return a null() identifier::NodeID.
     * @param view Variable description (MUST be valid)
     * @return identifier::NodeID identifying the requested Variable.
     */
    [[nodiscard]] identifier::NodeID find_id(view::VariableBackendView const &view) const noexcept;

    /**
     * Lookup the view::IRIBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return view::IRIBackendView describing the requested resource.
     */
    [[nodiscard]] view::IRIBackendView find_iri_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the view::LiteralBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return view::LiteralBackendView describing the requested resource.
     */
    [[nodiscard]] view::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the view::BNodeBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return view::BNodeBackendView describing the requested resource.
     */
    [[nodiscard]] view::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the view::VariableBackendView for the given identifier::NodeID. If it doesn't exist, the method will throw.
     * @param id NodeID of the requested resource
     * @return view::VariableBackendView describing the requested resource.
     */
    [[nodiscard]] view::VariableBackendView find_variable_backend_view(identifier::NodeID id) const;
    /**
     * Lookup the view::IRIBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or view.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return view::IRIBackendView describing the requested resource.
     */
    [[nodiscard]] static view::IRIBackendView find_iri_backend_view(identifier::NodeBackendHandle handle);
    /**
     * Lookup the view::LiteralBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or view.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return view::LiteralBackendView describing the requested resource.
     */
    [[nodiscard]] static view::LiteralBackendView find_literal_backend_view(identifier::NodeBackendHandle handle);
    /**
     * Lookup the view::BNodeBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or view.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return view::BNodeBackendView describing the requested resource.
     */
    [[nodiscard]] static view::BNodeBackendView find_bnode_backend_view(identifier::NodeBackendHandle handle);
    /**
     * Lookup the view::VariableBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or view.node_storage_id() doesn't exist, the method will throw.
     * @param handle NodeBackendHandle of the requested resource
     * @return view::VariableBackendView describing the requested resource.
     */
    [[nodiscard]] static view::VariableBackendView find_variable_backend_view(identifier::NodeBackendHandle handle);

    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.
     * This includes also Literals which use this IRI as datatype.  </p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_iri(identifier::NodeID id);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_literal(identifier::NodeID id);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_bnode(identifier::NodeID id);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeID. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     */
    bool erase_variable(identifier::NodeID id);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.
     * This includes also Literals which use this IRI as datatype.  </p>
     * <p>If view.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_iri(identifier::NodeBackendHandle handle);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.
     * <p>If view.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_literal(identifier::NodeBackendHandle handle);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>If view.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_bnode(identifier::NodeBackendHandle handle);
    /**
     * <p>Use with caution!</p>
     * <p>Erase the backend for the given identifier::NodeBackendHandle. The user must make sure that no more Nodes exist which use the referenced resource.</p>
     * <p>If view.node_storage_id() doesn't exist, the method will throw.</p>
     * <p>This method is not implemented in reference_node_storage::ReferenceNodeStorageBackend.</p>
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     */
    static bool erase_variable(identifier::NodeBackendHandle handle);

    bool operator==(NodeStorage const &other) const noexcept = default;
    bool operator!=(NodeStorage const &other) const noexcept = default;
};

/**
 * WeakNodeStorage is to NodeStorage what std::weak_ptr is to std::shared_ptr, i.e. while WeakNodeStorage
 * also points to a INodeStorageBackend (like a NodeStorage) it does not contribute to its reference count and
 * therefore does not keep it alive.
 */
struct WeakNodeStorage {
private:
    friend class NodeStorage;
    friend struct std::hash<WeakNodeStorage>;

    identifier::NodeStorageID backend_index;
    size_t generation;

    WeakNodeStorage(identifier::NodeStorageID backend_index, size_t generation) noexcept;
public:
    /**
     * Identifier of this NodeStorage
     * @return
     */
    [[nodiscard]] identifier::NodeStorageID id() const noexcept;

    /**
     * Tries to upgrade this WeakNodeStorage into a NodeStorage.
     * This will only succeed if the corresponding INodeStorageBackend is still alive.
     *
     * @return a NodeStorage pointing to the same backend as this, if the backend is still alive, otherwise nullopt
     */
    [[nodiscard]] std::optional<NodeStorage> try_upgrade() const noexcept;

    /**
     * Tries to upgrade this WeakNodeStorage into a NodeStorage.
     * This will only succeed if the corresponding INodeStorageBackend is still alive.
     * This function throws on failure to upgrade.
     *
     * @return a NodeStorage pointing to the same backend as this, if the backend is still alive
     * @throws std::runtime_error on upgrade failure
     */
    [[nodiscard]] NodeStorage upgrade() const;

    bool operator==(WeakNodeStorage const &) const noexcept = default;
    bool operator!=(WeakNodeStorage const &) const noexcept = default;
};

}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_NODESTORAGE_HPP
