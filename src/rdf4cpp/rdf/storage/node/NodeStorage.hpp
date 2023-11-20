#ifndef RDF4CPP_NODESTORAGE_HPP
#define RDF4CPP_NODESTORAGE_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>

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

/**
 * The control block for managing a single backend instance.
 * The reasoning for the atomicity of the fields can be found
 * in the first comment inside NodeStorage.
 */
struct ControlBlock {
    std::atomic<INodeStorageBackend *> backend{nullptr};
    std::atomic<size_t> refcount{0};
    std::atomic<size_t> generation{0};
};
}  //namespace node_storage_detail

/**
 * NodeStorage provides an interface to the internal storage for nodes.
 * Each NodeStorage has a INodeStorageBackend, which is uniquely identified by a identifier::NodeStorageID.
 * There can be at most 1023 different INodeStorageBackend instances at once.
 * If no NodeStorage with the identifier::NodeStorageID of a certain INodeStorageBackend exists anymore, the corresponding INodeStorageBackend is destructed.
 * This does not apply to the default_instance.
 * The lifecycle of NodeStorage's and their backends is managed automatically by this class, similar to a shared_ptr.
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
    static inline constinit std::array<node_storage_detail::ControlBlock, 1023> node_context_instances{};
    /**
     * Makes sure the default_instance_ is initialized only once
     */
    static inline std::once_flag default_init_once_flag;
    /**
     * THe default instance used whenever no NodeStorage is explicitly provided.
     */
    static NodeStorage default_instance_;

    /**
     * Index of the backend this NodeStorage is referring to.
     */
    identifier::NodeStorageID backend_index = node_storage_detail::INVALID_BACKEND_INDEX;

    /**
     * A cache for get_slot(backend_index).backend to elide
     * an atomic load for every backend access.
     */
    INodeStorageBackend *cached_backend_ptr;

    /**
     * Constructs a NodeStorage pointing to no instance
     */
    NodeStorage() noexcept = default;

    /**
     * Constructs a NodeStorage with the given backend index.
     * @param backend_index backend instance
     * @safety This function is only safe to call if you can guarantee that the reference count of the backend at backend_index can _not_
     *      reach zero while this constructed node storage is alive.
     */
    explicit NodeStorage(identifier::NodeStorageID backend_index) noexcept;

    /**
     * Constructs a NodeStorage with the given backend index and instance
     * @param backend_index the index to the backend slot
     * @param cached_backend_ptr the backend residing in that slot
     * @safety This function is only safe to call if you can guarantee that the reference count of the backend at backend_index can _not_
     *      reach zero while this constructed node storage is alive. Additionally cached_backend_ptr must be equal to get_slot(backend_index).backend
     */
    NodeStorage(identifier::NodeStorageID backend_index, INodeStorageBackend *cached_backend_ptr) noexcept;

    /**
     * Retrives the control block at the slot with the given id
     * @param id id of the slot
     * @return the corresponding control block
     */
    [[nodiscard]] inline static node_storage_detail::ControlBlock &get_slot(identifier::NodeStorageID id) noexcept {
        assert(id != node_storage_detail::INVALID_BACKEND_INDEX);
        return NodeStorage::node_context_instances[static_cast<size_t>(id.value)];
    }

    /**
     * Increases the reference count of the backend instance pointed to by
     * this NodeStorage.
     *
     * @safety This function must only be called if you can guarantee
     *      that the reference count of this backend cannot reach zero while calling this function.
     *      I.e. Something like NodeStorage{id}.increase_refcount(); is inherently undefined behaviour.
     */
    void increase_refcount() noexcept;

    /**
     * Decreases the reference count of the backend instance pointed to by
     * this NodeStorage.
     *
     * @safety This function must only be called on destruction or similar events (like assignment)
     *      calling it in any other scenario is undefined behaviour as it could destroy the backend
     *      other or this NodeStorage is referring to.
     */
    void decrease_refcount() noexcept;

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
    static void set_default_instance(NodeStorage const &node_context);

    /**
     * Create a NodeStorage with a custom Backend.
     * @tparam BackendImpl Class deriving from INodeStorage
     * @param args arguments to construct BackendImpl
     * @return NodeStorage referring to the instance.
     */
    template<typename BackendImpl, typename... Args> requires std::derived_from<BackendImpl, INodeStorageBackend>
    static NodeStorage new_instance(Args &&...args) {
        return register_backend(new BackendImpl{std::forward<Args>(args)...});
    }

    /**
     * Attempt to create a NodeStorage with a custom Backend at a specific id
     * @tparam BackendImpl Class deriving from INodeStorage
     * @param id id to assign the node storage
     * @param args arguments to construct BackendImpl
     * @return NodeStorage referring to the instance.
     */
    template<typename BackendImpl, typename ...Args> requires std::derived_from<BackendImpl, INodeStorageBackend>
    static NodeStorage new_instance_at(identifier::NodeStorageID id, Args &&...args) {
        return register_backend_at(id, new BackendImpl{std::forward<Args>(args)...});
    }

    /**
     * Create a new instance of reference_node_storage::ReferenceNodeStorageBackend.
     * @return NodeStorage backed by reference_node_storage::ReferenceNodeStorageBackend
     */
    static NodeStorage new_instance();

    /**
     * Retrieve NodeStorage for the given ID. If no such NodeStorage exists or the backend is already dead, std::nullopt is returned.
     * @param id NodeStorage id
     * @return optional NodeStorage
     */
    static std::optional<NodeStorage> lookup_instance(identifier::NodeStorageID id) noexcept;

    /**
     * Registers a INodeStorageBackend.
     *
     * @param backend_instance instance to be registered
     * @return an NodeStorage encapsulating backend_instance
     * @throws std::invalid_argument if a nullptr is provided
     * @throws std::length_error if the maximum number of backend instances has been exceeded
     * @safety This function takes ownership of the backend, do _not_ delete it manually or use it directly in any way
     *      after calling this function.
     */
    static NodeStorage register_backend(INodeStorageBackend *&&backend_instance);

    /**
     * Attempts to register a INodeStorageBackend at a specific id
     *
     * @param id id to assign the instance
     * @param backend_instance instance to be registered
     * @return an NodeStorage encapsulating backend_instance
     * @throws std::invalid_argument if a nullptr is provided
     * @throws std::logic_error if the given id is already in use
     * @safety This function takes ownership of the backend, do _not_ delete it manually or use it directly in any way
     *      after calling this function.
     */
     static NodeStorage register_backend_at(identifier::NodeStorageID id, INodeStorageBackend *&&backend_instance);

public:
    NodeStorage(NodeStorage &&other) noexcept;
    NodeStorage(const NodeStorage &node_context) noexcept;
    NodeStorage &operator=(const NodeStorage &other) noexcept;
    NodeStorage &operator=(NodeStorage &&other) noexcept;

    ~NodeStorage();

    /**
     * NodeStorage does instance reference counting. If no instances of a NodeStorage exist anymore its backend is destroyed.
     * For the default_instance an additional instance is kept so that its not even then destroyed if the there are no application held instances left.
     * @return current number of instances of this NodeStorage
     * @safety this function is inherently racy as the reference count can even change between fetching it and looking at the value
     *      do _not_ use it to check the reference count for _any_ kind of synchronization.
     */
    [[nodiscard]] size_t ref_count() const noexcept;

    /**
     * Number of nodes currently allocated by this node storage.
     * @return
     */
    [[nodiscard]] size_t size() const noexcept;

    /**
     * Checks if the current backend has specialized storage for the given datatype
     */
    [[nodiscard]] bool has_specialized_storage_for(identifier::LiteralType datatype) const noexcept;

    /**
     * Checks if the backend at id has specialized storage for the given datatype
     */
    [[nodiscard]] static bool has_specialized_storage_for(identifier::NodeStorageID id, identifier::LiteralType datatype) noexcept;

    /**
     * @return Identifier of this NodeStorage
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
     * Lookup the view::IRIBackendView for the given identifier::NodeBackendHandle.
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the requested resource
     * @return view::IRIBackendView describing the requested resource.
     * @throws std::exception if there is no backend at handle.node_storage_id()
     * @safety If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *      it is undefined from which node storage the node will actually be fetched, if any.
     */
    [[nodiscard]] static view::IRIBackendView find_iri_backend_view(identifier::NodeBackendHandle handle);

    /**
     * Lookup the view::LiteralBackendView for the given identifier::NodeBackendHandle. If it doesn't exist or view.node_storage_id() doesn't exist, the method will throw.
     *
     * @param handle NodeBackendHandle of the requested resource
     * @return view::LiteralBackendView describing the requested resource.
     * @throws std::exception if there is no backend at handle.node_storage_id()
     * @safety If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *      it is undefined from which node storage the node will actually be fetched, if any.
     */
    [[nodiscard]] static view::LiteralBackendView find_literal_backend_view(identifier::NodeBackendHandle handle);

    /**
     * Lookup the view::BNodeBackendView for the given identifier::NodeBackendHandle.
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the requested resource
     * @return view::IRIBackendView describing the requested resource.
     * @throws std::exception if there is no backend at handle.node_storage_id()
     * @safety If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *      it is undefined from which node storage the node will actually be fetched, if any.
     */
    [[nodiscard]] static view::BNodeBackendView find_bnode_backend_view(identifier::NodeBackendHandle handle);

    /**
     * Lookup the view::VariableBackendView for the given identifier::NodeBackendHandle.
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the requested resource
     * @return view::IRIBackendView describing the requested resource.
     * @throws std::exception if there is no backend at handle.node_storage_id()
     * @safety If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *      it is undefined from which node storage the node will actually be fetched, if any.
     */
    [[nodiscard]] static view::VariableBackendView find_variable_backend_view(identifier::NodeBackendHandle handle);

    /**
     * Erases the iri backend for the given identifier::NodeID.
     *
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     * @safety The user must make sure that no more Nodes exist which use the referenced resource, this also applies
     *      to literals using this iri as a datatype.
     */
    bool erase_iri(identifier::NodeID id);

    /**
     * Erase the literal backend for the given identifier::NodeID.
     *
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     * @safety The user must make sure that no more Nodes exist which use the referenced resource.
     */
    bool erase_literal(identifier::NodeID id);

    /**
     * Erase the literal backend for the given identifier::NodeID.
     *
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     * @safety The user must make sure that no more Nodes exist which use the referenced resource.
     */
    bool erase_bnode(identifier::NodeID id);

    /**
     * Erase the literal backend for the given identifier::NodeID.
     *
     * @param id NodeID of the resource to be erased
     * @return if a resource was erased
     * @safety The user must make sure that no more Nodes exist which use the referenced resource.
     */
    bool erase_variable(identifier::NodeID id);

    /**
     * Erase the iri backend for the given identifier::NodeBackendHandle.
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     * @throws std::exception if the NodeStorage for handle.node_storage_id() does not exist
     * @safety
     *      - The user must make sure that no more Nodes exist which use the referenced resource, this
     *          also applies to any literals using the iri as a datatype.
     *      - If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *          it is undefined from which node storage the node will actually be removed, if any.
     */
    static bool erase_iri(identifier::NodeBackendHandle handle);

    /**
     * Erase the literal backend for the given identifier::NodeBackendHandle.
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     * @throws std::exception if the NodeStorage for handle.node_storage_id() does not exist
     * @safety
     *      - The user must make sure that no more Nodes exist which use the referenced resource.
     *      - If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *          it is undefined from which node storage the node will actually be removed, if any.
     */
    static bool erase_literal(identifier::NodeBackendHandle handle);

    /**
     * Erases the bnode backend for the given identifier::NodeBackendHandle
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     * @throws std::exception if the NodeStorage for handle.node_storage_id() does not exist
     * @safety
     *      - The user must make sure that no more Nodes exist which use the referenced resource.
     *      - If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *          it is undefined from which node storage the node will actually be removed, if any.
     */
    static bool erase_bnode(identifier::NodeBackendHandle handle);

    /**
     * Erase the variable backend for the given identifier::NodeBackendHandle.
     * @warning Prefer the non-static version of this function whenever possible
     *      as it is inherently less racy.
     *
     * @param handle NodeBackendHandle of the resource to be erased
     * @return if a resource was erased
     * @throws std::exception if the NodeStorage for handle.node_storage_id() does not exist
     * @safety
     *      - The user must make sure that no more Nodes exist which use the referenced resource.
     *      - If there is currently no NodeStorage keeping the backend at handle.node_backend_id() alive
     *          it is undefined from which node storage the node will actually be removed, if any.
     */
    static bool erase_variable(identifier::NodeBackendHandle handle);

    /**
     * @return whether this and other are referring to the same backend
     */
    bool operator==(NodeStorage const &other) const noexcept;

    /**
     * @return whether this and other are _not_ referring to the same backend
     */
    bool operator!=(NodeStorage const &other) const noexcept;
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

    /**
     * @return whether this and other are referring to the same backend
     */
    bool operator==(WeakNodeStorage const &) const noexcept = default;

    /**
     * @return whether this and other are _not_ referring to the same backend
     */
    bool operator!=(WeakNodeStorage const &) const noexcept = default;
};

}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_NODESTORAGE_HPP
