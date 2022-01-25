#include "INodeStorageBackend.hpp"
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

namespace rdf4cpp::rdf::storage::node {
INodeStorageBackend::INodeStorageBackend()
    : manager_id(register_node_context(this)) {}
INodeStorageBackend::~INodeStorageBackend() {
    // unregister the object on destruction
    NodeStorage::node_context_instances[manager_id.value] = nullptr;
}
identifier::NodeStorageID INodeStorageBackend::register_node_context(INodeStorageBackend *node_context_backend) {
    // TODO: make sure an error it throw when we run out of manager spots.
    identifier::NodeStorageID manager_id{static_cast<uint16_t>(std::distance(NodeStorage::node_context_instances.begin(), std::find(NodeStorage::node_context_instances.begin(), NodeStorage::node_context_instances.end(), nullptr)))};
    NodeStorage::node_context_instances[manager_id.value] = node_context_backend;
    return manager_id;
}
void INodeStorageBackend::inc_use_count() noexcept {
    ++use_count_;
}
void INodeStorageBackend::dec_use_count() noexcept {
    --use_count_;
    // TODO destruct?
}
void INodeStorageBackend::inc_nodes_in_use() noexcept {
    ++nodes_in_use_;
}
void INodeStorageBackend::dec_nodes_in_use() noexcept {
    --nodes_in_use_;
    if (is_unreferenced())
        delete this;
}
bool INodeStorageBackend::is_unreferenced() const noexcept {
    // TODO: consider nodes_in_use as well
    return (use_count_ == 0);
}
size_t INodeStorageBackend::use_count() const noexcept {
    return use_count_;
}
size_t INodeStorageBackend::nodes_in_use() const noexcept {
    return nodes_in_use_;
}
}  // namespace rdf4cpp::rdf::storage::node