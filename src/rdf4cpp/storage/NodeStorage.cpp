#include <rdf4cpp/storage/NodeStorage.hpp>
#include <rdf4cpp/storage/reference_node_storage/SyncReferenceNodeStorage.hpp>

namespace rdf4cpp::storage {

namespace reference_node_storage {
SyncReferenceNodeStorage default_instance{};
} // reference_node_storage

#ifndef DOXYGEN_PARSER
DynNodeStoragePtr default_node_storage{reference_node_storage::default_instance};
#endif // DOXYGEN_PARSER

} // namespace rdf4cpp::storage
