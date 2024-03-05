#include "NodeStorage.hpp"

#include <rdf4cpp/storage/reference_node_storage/SyncReferenceNodeStorage.hpp>

namespace rdf4cpp::storage {

reference_node_storage::SyncReferenceNodeStorage default_node_storage_holder_;
DynNodeStorage default_node_storage{default_node_storage_holder_};

void reset_default_node_storage() noexcept {
    default_node_storage = default_node_storage_holder_;
}

} // namespace rdf4cpp::storage
