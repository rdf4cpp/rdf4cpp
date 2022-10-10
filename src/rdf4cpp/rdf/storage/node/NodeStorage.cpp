#include "NodeStorage.hpp"

namespace rdf4cpp::rdf::storage::node {
NodeStorage NodeStorage::default_instance_ = {};
INodeStorageBackend *NodeStorage::lookup_backend_instance(identifier::NodeStorageID id) {
    std::call_once(default_init_once_flag, []() {
        if (node_context_instances[0] == nullptr) {
            default_instance_ = new_instance();
            default_node_context_id = default_instance_.id();
            node_context_instances[0] = default_instance_.backend_;
        } else {
            node_context_instances[0]->inc_use_count();
            default_instance_ = NodeStorage(node_context_instances[0]);
            default_node_context_id = node_context_instances[0]->manager_id;
        }
    });
    return node_context_instances[id.value];
}
NodeStorage &NodeStorage::default_instance() {
    std::call_once(default_init_once_flag, []() {
        if (node_context_instances[0] == nullptr) {
            default_instance_ = new_instance();
            default_node_context_id = default_instance_.id();
            node_context_instances[0] = default_instance_.backend_;
        } else {
            node_context_instances[0]->inc_use_count();
            default_instance_ = NodeStorage(node_context_instances[0]);
            default_node_context_id = node_context_instances[0]->manager_id;
        }
    });
    return default_instance_;
}
void NodeStorage::default_instance(const NodeStorage &node_context) {
    default_instance_ = node_context;
    default_node_context_id = node_context.id();
}
NodeStorage NodeStorage::new_instance() {
    return NodeStorage(new reference_node_storage::ReferenceNodeStorageBackend());
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
std::optional<NodeStorage> NodeStorage::lookup_instance(identifier::NodeStorageID id) {
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
    this->backend_ = other.backend_;
    other.backend_ = nullptr;
}
NodeStorage::NodeStorage(const NodeStorage &other) noexcept : backend_(other.backend_) {
    this->backend_->inc_use_count();
}
NodeStorage &NodeStorage::operator=(const NodeStorage &other) noexcept {
    if (this != &other and this->backend_ != other.backend_) {
        if (this->backend_ != nullptr)
            this->backend_->dec_use_count();
        this->backend_ = other.backend_;
        if (this->backend_ != nullptr)
            this->backend_->inc_use_count();
    }
    return *this;
}
NodeStorage &NodeStorage::operator=(NodeStorage &&other) noexcept {
    if (this != &other) {
        if (this->backend_ != nullptr)
            this->backend_->dec_use_count();
        this->backend_ = other.backend_;
        other.backend_ = nullptr;
    }
    return *this;
}
size_t NodeStorage::use_count() const noexcept {
    return backend_->use_count_;
}
size_t NodeStorage::nodes_in_use() const noexcept {
    return backend_->nodes_in_use_;
}
identifier::NodeStorageID NodeStorage::id() const noexcept {
    return backend_->manager_id;
}

identifier::NodeID NodeStorage::find_or_make_id(const view::BNodeBackendView &view) noexcept {
    return backend_->find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_or_make_id(const view::IRIBackendView &view) noexcept {
    return backend_->find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_or_make_id(const view::LiteralBackendView &view) noexcept {
    return backend_->find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_or_make_id(const view::VariableBackendView &view) noexcept {
    return backend_->find_or_make_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::BNodeBackendView &view) const noexcept {
    return backend_->find_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::IRIBackendView &view) const noexcept {
    return backend_->find_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::LiteralBackendView &view) const noexcept {
    return backend_->find_id(view);
}
identifier::NodeID NodeStorage::find_id(const view::VariableBackendView &view) const noexcept {
    return backend_->find_id(view);
}
view::IRIBackendView NodeStorage::find_iri_backend_view(identifier::NodeID id) const {
    return backend_->find_iri_backend_view(id);
}
view::LiteralBackendView NodeStorage::find_literal_backend_view(identifier::NodeID id) const {
    return backend_->find_literal_backend_view(id);
}
view::BNodeBackendView NodeStorage::find_bnode_backend_view(identifier::NodeID id) const {
    return backend_->find_bnode_backend_view(id);
}
view::VariableBackendView NodeStorage::find_variable_backend_view(identifier::NodeID id) const {
    return backend_->find_variable_backend_view(id);
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
bool NodeStorage::erase_iri(identifier::NodeID id) const {
    return backend_->erase_iri(id);
}
bool NodeStorage::erase_literal(identifier::NodeID id) const {
    return backend_->erase_literal(id);
}
bool NodeStorage::erase_bnode(identifier::NodeID id) const {
    return backend_->erase_bnode(id);
}
bool NodeStorage::erase_variable(identifier::NodeID id) const {
    return backend_->erase_variable(id);
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


}  // namespace rdf4cpp::rdf::storage::node