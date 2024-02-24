#ifndef RDF4CPP_SYNCNODETYPESTORAGE_HPP
#define RDF4CPP_SYNCNODETYPESTORAGE_HPP

#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/UnsyncNodeTypeStorage.hpp>
#include <shared_mutex>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * Storage for one of the Node Backend types. Includes a shared mutex to synchronize access and bidirectional mappings between the Backend type and identifier::NodeID.
 * @tparam BackendType_t one of BNodeBackend, IRIBackend, FallbackLiteralBackend, SpecializedLiteralBackend and VariableBackend.
 */
template<typename BackendType_t>
struct SyncNodeTypeStorage : UnsyncNodeTypeStorage<BackendType_t> {
    using Base = UnsyncNodeTypeStorage<BackendType_t>;
    using Backend = typename Base::Backend;
    using BackendView = typename Base::BackendView;

    std::shared_mutex mutable mutex;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_SYNCNODETYPESTORAGE_HPP
