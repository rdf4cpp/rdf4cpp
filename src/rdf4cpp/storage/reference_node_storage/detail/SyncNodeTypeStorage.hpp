#ifndef RDF4CPP_SYNCNODETYPESTORAGE_HPP
#define RDF4CPP_SYNCNODETYPESTORAGE_HPP

#include "UnsyncNodeTypeStorage.hpp"
#include <shared_mutex>

namespace rdf4cpp::storage::reference_node_storage {

/**
 * Storage for one of the Node Backend types. Includes a shared mutex to synchronize access and bidirectional mappings between the Backend type and identifier::NodeID.
 * @tparam BackendType_t one of BNodeBackend, IRIBackend, FallbackLiteralBackend, SpecializedLiteralBackend and VariableBackend.
 */
template<typename BackendType_t>
struct SyncNodeTypeStorage : UnsyncNodeTypeStorage<BackendType_t> {
    using base_type = UnsyncNodeTypeStorage<BackendType_t>;
    using backend_type = typename base_type::backend_type;
    using backend_view_type = typename base_type::backend_view_type;

    std::shared_mutex mutable mutex;
};

}  // namespace rdf4cpp::storage::reference_node_storage

#endif  //RDF4CPP_SYNCNODETYPESTORAGE_HPP
