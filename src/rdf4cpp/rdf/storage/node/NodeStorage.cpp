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
    return NodeStorage(new default_node_storage::DefaultNodeStorageBackend());
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
identifier::NodeStorageID NodeStorage::id() const noexcept {
    return backend_->manager_id;
}
NodeStorage::NodeID NodeStorage::get_string_literal_id(std::string_view lexical_form) {
    return backend_->get_string_literal_id(lexical_form);
}
NodeStorage::NodeID NodeStorage::get_typed_literal_id(std::string_view lexical_form, std::string_view datatype) {
    return backend_->get_typed_literal_id(lexical_form, datatype);
}
NodeStorage::NodeID NodeStorage::get_typed_literal_id(std::string_view lexical_form, const NodeStorage::NodeID &datatype_id) {
    return backend_->get_typed_literal_id(lexical_form, datatype_id);
}
NodeStorage::NodeID NodeStorage::get_lang_literal_id(std::string_view lexical_form, std::string_view lang) {
    return backend_->get_lang_literal_id(lexical_form, lang);
}
NodeStorage::NodeID NodeStorage::get_iri_id(std::string_view iri) {
    return backend_->get_iri_id(iri);
}
NodeStorage::NodeID NodeStorage::get_variable_id(std::string_view identifier, bool anonymous) {
    return backend_->get_variable_id(identifier, anonymous);
}
NodeStorage::NodeID NodeStorage::get_bnode_id(std::string_view identifier) {
    return backend_->get_bnode_id(identifier);
}
handle::IRIBackendView NodeStorage::get_iri_handle(NodeStorage::NodeID id) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(id.manager_id());
    return backend->get_iri_handle(id.node_id());
}
handle::LiteralBackendView NodeStorage::get_literal_handle(NodeStorage::NodeID id) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(id.manager_id());
    return backend->get_literal_handle(id.node_id());
}
handle::BNodeBackendView NodeStorage::get_bnode_handle(NodeStorage::NodeID id) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(id.manager_id());
    return backend->get_bnode_handle(id.node_id());
}
handle::VariableBackendView NodeStorage::get_variable_handle(NodeStorage::NodeID id) {
    INodeStorageBackend *backend = NodeStorage::lookup_backend_instance(id.manager_id());
    return backend->get_variable_handle(id.node_id());
}
bool NodeStorage::operator==(const NodeStorage &other) const {
    return this->id() == other.id();
}
}  // namespace rdf4cpp::rdf::storage::node