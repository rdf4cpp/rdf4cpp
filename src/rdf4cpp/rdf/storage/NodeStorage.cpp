#include "NodeStorage.hpp"
#include "rdf4cpp/rdf/storage/reference_node_storage/SyncReferenceNodeStorageBackend.hpp"

namespace rdf4cpp::rdf::storage {

reference_node_storage::SyncReferenceNodeStorageBackend default_node_storage_holder_;

DynNodeStorage default_node_storage{default_node_storage_holder_};

} // namespace rdf4cpp::rdf::storage