#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceNodeScope.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::util {

/**
 * For general correctness reasoning, see NodeStorage. This implementation is very similar.
 */

void NodeScope::increase_refcount() noexcept {
    get_slot(this->backend_index_)
            .refcount
            .fetch_add(1, std::memory_order_relaxed);
}

void NodeScope::decrease_refcount() noexcept {
    auto &slot = get_slot(this->backend_index_);

    if (slot.refcount.fetch_sub(1, std::memory_order_release) != 1) {
        return;
    }

    std::atomic_thread_fence(std::memory_order_acquire);
    INodeScope *old = slot.backend.exchange(nullptr, std::memory_order_relaxed);
    delete old;
}

NodeScope::NodeScope(identifier::NodeScopeID backend_index) noexcept : backend_index_{backend_index},
                                                                       cached_backend_ptr_{get_slot(backend_index).backend.load(std::memory_order_relaxed)} {
    assert(cached_backend_ptr_ != nullptr);
}

NodeScope::NodeScope(identifier::NodeScopeID backend_index, INodeScope *cached_backend_ptr) noexcept : backend_index_{backend_index},
                                                                                                       cached_backend_ptr_{cached_backend_ptr} {
    assert(get_slot(backend_index).backend == cached_backend_ptr);
}

NodeScope::NodeScope(NodeScope const &other) noexcept : backend_index_{other.backend_index_},
                                                        cached_backend_ptr_{other.cached_backend_ptr_} {
    this->increase_refcount();
}

NodeScope::NodeScope(NodeScope &&other) noexcept : backend_index_{std::exchange(other.backend_index_, node_scope_detail::INVALID_BACKEND_INDEX)},
                                                   cached_backend_ptr_{std::exchange(other.cached_backend_ptr_, nullptr)}{
}

NodeScope &NodeScope::operator=(NodeScope const &other) noexcept {
    if (this != &other && this->backend_index_ != other.backend_index_) {
        if (this->backend_index_ != node_scope_detail::INVALID_BACKEND_INDEX) {
            this->decrease_refcount();
        }

        this->backend_index_ = other.backend_index_;
        this->cached_backend_ptr_ = other.cached_backend_ptr_;
        this->increase_refcount();
    }

    return *this;
}

NodeScope &NodeScope::operator=(NodeScope &&other) noexcept {
    if (this != &other) {
        if (this->backend_index_ != node_scope_detail::INVALID_BACKEND_INDEX) {
            this->decrease_refcount();
        }

        this->backend_index_ = std::exchange(other.backend_index_, node_scope_detail::INVALID_BACKEND_INDEX);
        this->cached_backend_ptr_ = std::exchange(other.cached_backend_ptr_, nullptr);
    }

    return *this;
}

NodeScope::~NodeScope() {
    if (backend_index_ == node_scope_detail::INVALID_BACKEND_INDEX) {
        return;
    }

    this->decrease_refcount();
}

NodeScope NodeScope::register_backend(INodeScope *&&backend_instance) {
    if (backend_instance == nullptr) [[unlikely]] {
        throw std::runtime_error{"Backend instance must not be null"};
    }

    for (size_t ix = 0; ix < NodeScope::node_scope_instances_.size(); ++ix) {
        auto &slot = NodeScope::node_scope_instances_[ix];

        INodeScope *old_value = nullptr;
        if (slot.backend.compare_exchange_strong(old_value, backend_instance, std::memory_order_release, std::memory_order_acquire)) {
            // found slot
            slot.generation.fetch_add(1, std::memory_order_release); // release for synchronization with lookup_instance
            slot.refcount.store(1, std::memory_order_relaxed);

            return NodeScope{identifier::NodeScopeID{static_cast<uint16_t>(ix)}, backend_instance};
        } else if (old_value == backend_instance) [[unlikely]] {
            throw std::runtime_error{"The provided backend is already registered"};
        }
    }

    throw std::runtime_error{"Maximum number of backend instances exceeded"};
}

NodeScope NodeScope::new_instance() {
    return register_backend(new ReferenceNodeScope{});
}

std::optional<NodeScope> NodeScope::lookup_instance(identifier::NodeScopeID id) noexcept {
    auto &slot = get_slot(id);

    auto old_rc = slot.refcount.load(std::memory_order_relaxed);
    while (true) {
        if (old_rc == 0) {
            return std::nullopt;
        }

        if (slot.refcount.compare_exchange_weak(old_rc, old_rc + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
            assert(slot.backend != nullptr);
            assert(slot.refcount > 0);

            return NodeScope{id};
        }
    }
}

size_t NodeScope::ref_count() const noexcept {
    return get_slot(this->backend_index_)
            .refcount
            .load(std::memory_order_acquire);
}

identifier::NodeScopeID NodeScope::id() const noexcept {
    return this->backend_index_;
}

WeakNodeScope NodeScope::downgrade() const noexcept {
    auto const cur_gen = get_slot(this->backend_index_)
                                 .generation
                                 .load(std::memory_order_relaxed);

    return WeakNodeScope{this->backend_index_, cur_gen};
}

Node NodeScope::generate_node(NodeGenerator &generator, NodeStorage &node_storage) const {
    return generator.generate_node_impl(this, node_storage);
}

Node NodeScope::try_get_node(std::string_view label) const noexcept {
    return Node{this->cached_backend_ptr_->find_node(label)};
}

Node NodeScope::get_or_generate_node(std::string_view label, NodeGenerator &generator, NodeStorage &node_storage) {
    if (Node node = this->try_get_node(label); !node.null()) {
        return node;
    }

    Node node = this->generate_node(generator, node_storage);
    this->cached_backend_ptr_->label_node(label, node.backend_handle());

    return node;
}

std::optional<std::string_view> NodeScope::try_get_label(Node const &node) const noexcept {
    return this->cached_backend_ptr_->find_label(node.backend_handle());
}

bool NodeScope::operator==(NodeScope const &other) const noexcept {
    return this->backend_index_ == other.backend_index_;
}

bool NodeScope::operator!=(NodeScope const &other) const noexcept {
    return this->backend_index_ != other.backend_index_;
}

WeakNodeScope::WeakNodeScope(identifier::NodeScopeID backend_index, size_t generation) noexcept : backend_index_{backend_index},
                                                                                                  generation_{generation} {
}

identifier::NodeScopeID WeakNodeScope::id() const noexcept {
    return this->backend_index_;
}

std::optional<NodeScope> WeakNodeScope::try_upgrade() const noexcept {
    auto &slot = NodeScope::get_slot(this->backend_index_);

    if (slot.generation.load(std::memory_order_acquire) != this->generation_) {
        return std::nullopt;
    }

    auto old_rc = slot.refcount.load(std::memory_order_relaxed);
    while (true) {
        if (old_rc == 0) {
            return std::nullopt;
        }

        if (slot.refcount.compare_exchange_weak(old_rc, old_rc + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
            assert(slot.backend != nullptr);
            assert(slot.generation == this->generation_);
            return NodeScope{this->backend_index_};
        }
    }
}

NodeScope WeakNodeScope::upgrade() const {
    if (auto ns = this->try_upgrade(); ns.has_value()) {
        return *ns;
    }

    throw std::runtime_error{"WeakNodeScope lifetime error: referenced backend is no longer alive"};
}

}  //namespace rdf4cpp::rdf::util

size_t std::hash<rdf4cpp::rdf::util::NodeScope>::operator()(rdf4cpp::rdf::util::NodeScope const &scope) const noexcept {
    return rdf4cpp::rdf::storage::util::robin_hood::hash<uint16_t>{}(scope.backend_index_.to_underlying());
}

size_t std::hash<rdf4cpp::rdf::util::WeakNodeScope>::operator()(rdf4cpp::rdf::util::WeakNodeScope const &scope) const noexcept {
    return rdf4cpp::rdf::storage::util::robin_hood::hash<std::array<size_t, 2>>{}(std::array<size_t, 2>{
            rdf4cpp::rdf::storage::util::robin_hood::hash<uint16_t>{}(scope.backend_index_.to_underlying()),
            rdf4cpp::rdf::storage::util::robin_hood::hash<size_t>{}(scope.generation_)});
}
