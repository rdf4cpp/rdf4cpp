#include "NodeStorage.hpp"

namespace rdf4cpp::rdf::storage::node {
NodeStorage NodeStorage::default_instance_ = {};
INodeStorageBackend *NodeStorage::lookup_backend_instance(NodeStorageID id) {
    std::call_once(default_init_once_flag, []() {
        if (node_context_instances[0] == nullptr) {
            default_instance_ = new_instance();
            default_node_context_id = default_instance_.id();
            node_context_instances[0] = default_instance_.backend_;
        } else {
            default_instance_ = NodeStorage(node_context_instances[0]);
            default_node_context_id = node_context_instances[0]->manager_id;
        }
    });
    return node_context_instances[id.value];
}
NodeStorage &NodeStorage::primary_instance() {
    std::call_once(default_init_once_flag, []() {
        if (node_context_instances[0] == nullptr) {
            default_instance_ = new_instance();
            default_node_context_id = default_instance_.id();
            node_context_instances[0] = default_instance_.backend_;
        } else {
            default_instance_ = NodeStorage(node_context_instances[0]);
            default_node_context_id = node_context_instances[0]->manager_id;
        }
    });
    return default_instance_;
}
void NodeStorage::primary_instance(const NodeStorage &node_context) {
    default_instance_ = node_context;
    default_node_context_id = node_context.id();
}
NodeStorage NodeStorage::new_instance() {
    return NodeStorage(new DefaultNodeStorageBackend());
}
NodeStorage NodeStorage::register_backend(INodeStorageBackend *backend_instance) {
    if (backend_instance == nullptr)
        throw std::runtime_error("Backend instance must not be null.");

    auto &stored_instance = node_context_instances[backend_instance->manager_id.value];

    if (stored_instance != backend_instance and stored_instance != nullptr)
        throw std::runtime_error("A NodeStorage with manager_id " + std::to_string(backend_instance->manager_id.value) + " is already registered.");

    node_context_instances[backend_instance->manager_id.value] = backend_instance;
    return NodeStorage(backend_instance);
}
void NodeStorage::unregister_backend(INodeStorageBackend *backend_instance) {
    if (backend_instance == nullptr)
        throw std::runtime_error("Backend instance must not be null.");
    node_context_instances[backend_instance->manager_id.value] = nullptr;
}
std::optional<NodeStorage> NodeStorage::lookup_instance(NodeStorageID id) {
    // TODO: would be better if we returned a reference here, i.e. std::optional<std::reference_wrapper<NodeStorage>>
    INodeStorageBackend *backend = lookup_backend_instance(id);
    if (backend != nullptr) {
        backend->inc_use_count();
        return NodeStorage(backend);
    } else {
        return std::nullopt;
    }
}
NodeStorage::~NodeStorage() {
    if (backend_ != nullptr) backend_->dec_use_count();
}
NodeStorage::NodeStorage(NodeStorage &&other) noexcept {
    if (this->backend_ != other.backend_) {
        if (this->backend_ != nullptr)
            this->backend_->dec_use_count();
        this->backend_ = other.backend_;
        if (this->backend_ != nullptr)
            this->backend_->inc_use_count();
    } else {
        if (other.backend_ != nullptr) {
            other.backend_->dec_use_count();
        }
    }

    other.backend_ = nullptr;
}
NodeStorage::NodeStorage(const NodeStorage &node_context) noexcept : backend_(node_context.backend_) {
    if (this->backend_ != nullptr) node_context.backend_->inc_use_count();
}
NodeStorage &NodeStorage::operator=(const NodeStorage &other) noexcept {
    if (this == &other)
        return *this;
    else if (this->backend_ != other.backend_) {
        if (this->backend_ != nullptr)
            this->backend_->dec_use_count();
        this->backend_ = other.backend_;
        if (this->backend_ != nullptr)
            this->backend_->inc_use_count();
    }
    return *this;
}
NodeStorage &NodeStorage::operator=(NodeStorage &&other) noexcept {
    if (this->backend_ != other.backend_) {
        if (this->backend_ != nullptr)
            this->backend_->dec_use_count();
        this->backend_ = other.backend_;
        if (this->backend_ != nullptr)
            this->backend_->inc_use_count();
    } else {
        if (other.backend_ != nullptr) {
            other.backend_->dec_use_count();
        }
    }

    other.backend_ = nullptr;
    return *this;
}
size_t NodeStorage::use_count() const noexcept {
    return backend_->use_count_;
}
size_t NodeStorage::nodes_in_use() const noexcept {
    return backend_->nodes_in_use_;
}
NodeStorageID NodeStorage::id() const noexcept {
    return backend_->manager_id;
}
}  // namespace rdf4cpp::rdf::storage::node