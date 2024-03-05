#include <rdf4cpp/bnode_mngt/WeakNodeScope.hpp>
#include <rdf4cpp/bnode_mngt/NodeScope.hpp>

namespace rdf4cpp::bnode_mngt {

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

} // namespace rdf4cpp::bnode_mngt