#include "NodeStorage.hpp"

#include <algorithm>

namespace rdf4cpp::rdf::storage::node {
NodeStorage NodeStorage::default_instance_ = {};
INodeStorageBackend *NodeStorage::lookup_backend_instance(identifier::NodeStorageID id) {
    std::call_once(default_init_once_flag, []() {
        default_instance_ = new_instance();
    });

    return node_context_instances[id.value].backend;
}
NodeStorage &NodeStorage::default_instance() {
    std::call_once(default_init_once_flag, []() {
        default_instance_ = new_instance();
    });

    return default_instance_;
}
void NodeStorage::default_instance(NodeStorage const &node_context) {
    default_instance_ = node_context;
}

NodeStorage NodeStorage::new_instance() {
    return register_backend(new reference_node_storage::ReferenceNodeStorageBackend());
}

NodeStorage NodeStorage::register_backend(INodeStorageBackend *backend_instance) {
    if (backend_instance == nullptr) {
        throw std::runtime_error("Backend instance must not be null.");
    }

    /**
     * We will now try to reserve a slot for ourselves.
     * This is akin to lazily initializing a singleton, except that we have to try multiple slots.
     * Here we need to make sure that reading the current state of the pointer does not race with setting it
     * to its desired value.
     *
     * This means we need to load-acquire the data current and then release-store the new data.
     *
     * If another thread is racing for the same slot and it wins,
     * the pointer will no longer be nullptr and the compare_exchange will fail
     * and acquire-load the new value, we don't care about this value and will simply retry on the next slot.
     *
     * If we win the race we store the pointer with a release-store in which
     * case the compare_exchange of all other threads racing for this slot will fail.
     *
     * Storing the refcount with a relaxed-store is fine here since there can be no
     * other threads racing for the same reference count at this point. This is
     * because there can currently be no other NodeStorages pointing to this slot, since by definition this
     * execution is the one that creates the first one pointing to this slot in this generation.
     * Additionally existing WeakNodeStorages pointing to this slot are not interested
     * in it as the generation increases here with a release-store which is synchronized with Weak::upgrade.
     *
     * see https://marabos.nl/atomics/memory-ordering.html#example-lazy-initialization-with-indirection
     */
    for (size_t ix = 0; ix < NodeStorage::node_context_instances.size(); ++ix) {
        auto &slot = NodeStorage::node_context_instances[ix];

        INodeStorageBackend *old_value = nullptr;
        if (slot.backend.compare_exchange_strong(old_value, backend_instance, std::memory_order_release, std::memory_order_acquire)) {
            // found slot
            slot.generation.fetch_add(1, std::memory_order_release); // release for synchronization with Weak::upgrade
            slot.refcount.store(1, std::memory_order_relaxed);

            return NodeStorage{identifier::NodeStorageID{static_cast<uint16_t>(ix)}};
        } else if (old_value == backend_instance) {
            throw std::runtime_error{"The provided backend is already registered"};
        }
    }

    throw std::runtime_error{"Maximum number of backend instances exceeded"};
}

void NodeStorage::unregister_backend(INodeStorageBackend *backend_instance) {
    if (backend_instance == nullptr) {
        throw std::runtime_error("Backend instance must not be null.");
    }

    auto it = std::find_if(NodeStorage::node_context_instances.begin(), NodeStorage::node_context_instances.end(), [&](auto const &slot) {
        return slot.backend == backend_instance;
    });

    if (it == NodeStorage::node_context_instances.end()) {
        throw std::runtime_error{"Backend instance not found"};
    }

    // TODO: just remove this function? It is basically just always UB when the reference count
    // is not 0, and if it is zero calling this function is useless.
    // And I'm not sure what the purpose of this function is.
    assert(false);
}

std::optional<NodeStorage> NodeStorage::lookup_instance(identifier::NodeStorageID id) {
    if (lookup_backend_instance(id) == nullptr) {
        return std::nullopt;
    }

    /**
     * Release-store to synchronize with acquire-load in decrease_refcount.
     * As there is currently no guaranteed object of NodeStorage alive
     * we need to make sure that this reference count increase happens before
     * the potentially running destructor deletes the backend.
     */
    get_slot(id)
            .refcount
            .fetch_add(1, std::memory_order_release);

    return NodeStorage{id};
}

NodeStorage::NodeStorage(identifier::NodeStorageID backend_index) noexcept : backend_index{backend_index} {
}

NodeStorage::~NodeStorage() {
    if (this->backend_index == INVALID_BACKEND_INDEX) {
        return;
    }

    this->decrease_refcount();
}
NodeStorage::NodeStorage(NodeStorage &&other) noexcept : backend_index{other.backend_index} {
    other.backend_index = INVALID_BACKEND_INDEX;
}

NodeStorage::NodeStorage(NodeStorage const &other) noexcept : backend_index{other.backend_index} {
    this->increase_refcount();
}

NodeStorage &NodeStorage::operator=(NodeStorage const &other) noexcept {
    if (this != &other && this->backend_index != other.backend_index) {
        if (this->backend_index != INVALID_BACKEND_INDEX) {
            this->decrease_refcount();
        }

        this->backend_index = other.backend_index;
        if (this->backend_index != INVALID_BACKEND_INDEX) {
            this->increase_refcount();
        }
    }

    return *this;
}

NodeStorage &NodeStorage::operator=(NodeStorage &&other) noexcept {
    if (this != &other) {
        if (this->backend_index != INVALID_BACKEND_INDEX) {
            this->decrease_refcount();
        }

        this->backend_index = other.backend_index;
        other.backend_index = INVALID_BACKEND_INDEX;
    }

    return *this;
}

void NodeStorage::decrease_refcount() noexcept {
    auto &slot = get_slot(this->backend_index);

    /**
     * All memory accesses must happen before this store because it is
     * a release-store that prevents preceding memory ops to be reordered past itself.
     */
    if (slot.refcount.fetch_sub(1, std::memory_order_release) != 1) {
        return;
    }

    /**
     * Deletion of data cannot be reordered before this fence.
     * Refcount decrease cannot be reordered after this fence as release->acquire.
     *
     * So:
     * data access -> refcount decrease -> fence -> (reset to nullptr & deletion)
     */
    std::atomic_thread_fence(std::memory_order_acquire);

    /**
     * Can be a relaxed-store as using release only influences how fast this slot will be found.
     * Release would force register_backend to find it earlier as the acquire-read of the backend there would synchronize with the
     * release-store here.
     */
    INodeStorageBackend *old = slot.backend.exchange(nullptr, std::memory_order_relaxed);
    delete old;
}

void NodeStorage::increase_refcount() noexcept {
    /**
     * Can be relaxed as this object is keeping the backend alive anyways.
     *
     * see https://www.boost.org/doc/libs/1_57_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters
     */
    get_slot(this->backend_index)
            .refcount
            .fetch_add(1, std::memory_order_relaxed);
}

WeakNodeStorage NodeStorage::downgrade() const noexcept {
    /**
     * We want to definitely read the current generation
     * value and not an increased one from the backend being replaced.
     * But this can only happen in the following circumstance, which is impossible
     * without explicitly calling a destructor.
     *
     * Working on a specific NodeStorage n with backend index ix:
     *
     * THREAD A        |  THREAD B
     * ---------------------------------------------------
     *                 |  n.~NodeStorage()
     *                 |  register_backend on ix
     *                 |      write new backend into slot
     *                 |      increase generation
     *  n.downgrade()  |
     *
     * You then get a weak to the new allocation instead of an invalid one to the old one.
     * But you also needed to call a method on a destroyed object for that to occur which is UB anyways.
     */
    auto const cur_gen = get_slot(this->backend_index)
                                 .generation
                                 .load(std::memory_order_relaxed);

    return WeakNodeStorage{this->backend_index, cur_gen};
}

size_t NodeStorage::ref_count() const noexcept {
    if (this->backend_index == INVALID_BACKEND_INDEX) {
        return 0;
    }

    return get_slot(this->backend_index)
            .refcount
            .load(std::memory_order_acquire);
}

size_t NodeStorage::size() const noexcept {
    return get_backend().size();
}

identifier::NodeStorageID NodeStorage::id() const noexcept {
    return this->backend_index;
}

identifier::NodeID NodeStorage::find_or_make_id(const view::BNodeBackendView &view) noexcept {
    return get_backend().find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_or_make_id(const view::IRIBackendView &view) noexcept {
    return get_backend().find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_or_make_id(const view::LiteralBackendView &view) noexcept {
    return get_backend().find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_or_make_id(const view::VariableBackendView &view) noexcept {
    return get_backend().find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::BNodeBackendView &view) const noexcept {
    return get_backend().find_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::IRIBackendView &view) const noexcept {
    return get_backend().find_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::LiteralBackendView &view) const noexcept {
    return get_backend().find_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::VariableBackendView &view) const noexcept {
    return get_backend().find_id(view);
}
view::IRIBackendView NodeStorage::find_iri_backend_view(identifier::NodeID id) const {
    return get_backend().find_iri_backend_view(id);
}
view::LiteralBackendView NodeStorage::find_literal_backend_view(identifier::NodeID id) const {
    return get_backend().find_literal_backend_view(id);
}
view::BNodeBackendView NodeStorage::find_bnode_backend_view(identifier::NodeID id) const {
    return get_backend().find_bnode_backend_view(id);
}
view::VariableBackendView NodeStorage::find_variable_backend_view(identifier::NodeID id) const {
    return get_backend().find_variable_backend_view(id);
}
view::IRIBackendView NodeStorage::find_iri_backend_view(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->find_iri_backend_view(handle.node_id());
}
view::LiteralBackendView NodeStorage::find_literal_backend_view(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->find_literal_backend_view(handle.node_id());
}
view::BNodeBackendView NodeStorage::find_bnode_backend_view(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->find_bnode_backend_view(handle.node_id());
}
view::VariableBackendView NodeStorage::find_variable_backend_view(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->find_variable_backend_view(handle.node_id());
}
bool NodeStorage::erase_iri(identifier::NodeID id) {
    return get_backend().erase_iri(id);
}
bool NodeStorage::erase_literal(identifier::NodeID id) {
    return get_backend().erase_literal(id);
}
bool NodeStorage::erase_bnode(identifier::NodeID id) {
    return get_backend().erase_bnode(id);
}
bool NodeStorage::erase_variable(identifier::NodeID id) {
    return get_backend().erase_variable(id);
}
bool NodeStorage::erase_iri(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->erase_iri(handle.node_id());
}
bool NodeStorage::erase_literal(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->erase_literal(handle.node_id());
}
bool NodeStorage::erase_bnode(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->erase_bnode(handle.node_id());
}
bool NodeStorage::erase_variable(identifier::NodeBackendHandle handle) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(handle.node_storage_id());
    return backend->erase_literal(handle.node_id());
}



WeakNodeStorage::WeakNodeStorage(identifier::NodeStorageID backend_index, size_t generation) noexcept
    : backend_index{backend_index}, generation{generation} {
}

identifier::NodeStorageID WeakNodeStorage::id() const noexcept {
    return this->backend_index;
}

std::optional<NodeStorage> WeakNodeStorage::try_upgrade() const noexcept {
    auto &slot = NodeStorage::get_slot(this->backend_index);

    /**
     * Load the current generation, this has to synchronize with
     * the generation increase in register_backend.
     *
     * Also prevents compare exchange loop below being reordered before
     * this instruction because it is an acquire-load.
     *
     * generation increase in register_backend -> generation load here -> refcount increase below
     */
    if (slot.generation.load(std::memory_order_acquire) != this->generation) {
        return std::nullopt;
    }

    /**
     * This function is not supposed to ever change a 0 refcount to anything
     * higher. But of course it is supposed to increase the reference count if
     * it is already greater than 0.
     *
     * Therefore we cannot use a simple fetch_add as it would increase the reference count from 0.
     * Instead we are doing a compare exchange loop and fail if the old value is 0.
     *
     * Relaxed-loads are fine here since in the failure case we do nothing and also do not care about
     * whatever state the refcount is in.
     */
    auto old_rc = slot.refcount.load(std::memory_order_relaxed);
    while (true) {
        if (old_rc == 0) {
            return std::nullopt;
        }

        if (slot.refcount.compare_exchange_weak(old_rc, old_rc + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
            return NodeStorage{this->backend_index};
        }
    }
}

NodeStorage WeakNodeStorage::upgrade() const {
    if (auto ns = this->try_upgrade(); ns.has_value()) {
        return *ns;
    }

    throw std::runtime_error{"WeakNodeStorage lifetime error: referenced backend is no longer alive"};
}

}  // namespace rdf4cpp::rdf::storage::node
