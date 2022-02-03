#include "INodeStorageBackend.hpp"
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

namespace rdf4cpp::rdf::storage::node {

std::mutex INodeStorageBackend::destruction_mutex_{};

INodeStorageBackend::INodeStorageBackend()
    : manager_id(register_node_context(this)) {}
INodeStorageBackend::~INodeStorageBackend() {
    // unregister the object on destruction
    NodeStorage::node_context_instances[manager_id.value] = nullptr;
}
identifier::NodeStorageID INodeStorageBackend::register_node_context(INodeStorageBackend *node_context_backend) {
    std::scoped_lock lock{destruction_mutex_};
    size_t next_manager_id = std::distance(NodeStorage::node_context_instances.begin(), std::find(NodeStorage::node_context_instances.begin(), NodeStorage::node_context_instances.end(), nullptr));
    if (next_manager_id >= NodeStorage::node_context_instances.size())
        throw std::runtime_error("The maximum number of INodeStorage instances has been exceeded.");
    identifier::NodeStorageID manager_id{static_cast<uint16_t>(next_manager_id)};
    NodeStorage::node_context_instances[manager_id.value] = node_context_backend;
    return manager_id;
}
void INodeStorageBackend::inc_use_count() noexcept {
    ++use_count_;
}
void INodeStorageBackend::dec_use_count() noexcept {
    std::scoped_lock lock{destruction_mutex_};
    if (--use_count_ == 0 and nodes_in_use_ == 0)
        delete this;
}
void INodeStorageBackend::inc_nodes_in_use() noexcept {
    ++nodes_in_use_;
}
void INodeStorageBackend::dec_nodes_in_use() noexcept {
    std::scoped_lock lock{destruction_mutex_};
    if (--nodes_in_use_ == 0 and use_count_ == 0)
        delete this;
}
size_t INodeStorageBackend::use_count() const noexcept {
    return use_count_;
}
size_t INodeStorageBackend::nodes_in_use() const noexcept {
    return nodes_in_use_;
}
}  // namespace rdf4cpp::rdf::storage::node