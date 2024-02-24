#include <rdf4cpp/rdf/bnode_mngt/NodeScope.hpp>
#include <rdf4cpp/rdf/bnode_mngt/reference_backends/scope/ReferenceNodeScope.hpp>
#include <dice/hash.hpp>

namespace rdf4cpp::rdf::bnode_mngt {

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
        throw std::invalid_argument{"Backend instance must not be null"};
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
            throw std::logic_error{"The provided backend is already registered"};
        }
    }

    delete backend_instance;
    throw std::length_error{"Maximum number of backend instances exceeded"};
}

NodeScope NodeScope::register_backend_at(identifier::NodeScopeID id, INodeScope *&&backend_instance) {
    if (backend_instance == nullptr) [[unlikely]] {
        throw std::invalid_argument{"Backend instance must not be null"};
    }

    auto &slot = NodeScope::node_scope_instances_[id.to_underlying()];

    INodeScope *old_value = nullptr;
    if (slot.backend.compare_exchange_strong(old_value, backend_instance, std::memory_order_release, std::memory_order_acquire)) [[likely]] {
        // found slot
        slot.generation.fetch_add(1, std::memory_order_release); // release for synchronization with lookup_instance
        slot.refcount.store(1, std::memory_order_relaxed);

        return NodeScope{id, backend_instance};
    }

    delete backend_instance;
    throw std::logic_error{"The node scope ID is already in use"};
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

}  //namespace rdf4cpp::rdf::bnode_mngt

size_t std::hash<rdf4cpp::rdf::bnode_mngt::NodeScope>::operator()(rdf4cpp::rdf::bnode_mngt::NodeScope const &scope) const noexcept {
    return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(scope.backend_index_.to_underlying());
}

size_t std::hash<rdf4cpp::rdf::bnode_mngt::WeakNodeScope>::operator()(rdf4cpp::rdf::bnode_mngt::WeakNodeScope const &scope) const noexcept {
    return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(
            std::make_pair(scope.backend_index_.to_underlying(), scope.generation_));
}
