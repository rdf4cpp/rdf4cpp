#ifndef RDF4CPP_NAMESPACEINSTANCE_HPP
#define RDF4CPP_NAMESPACEINSTANCE_HPP

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>

#include <cinttypes>

namespace rdf4cpp::rdf::namespaces::util {

template<typename NamespaceClass>
struct NamespaceInstance {
    static NamespaceClass &instance(storage::node::NodeStorage &node_storage = storage::node::NodeStorage::default_instance()) {
        static storage::util::tsl::sparse_map<uint16_t, NamespaceClass> instances;
        uint16_t const node_storage_id = node_storage.id().value;
        if (auto found = instances.find(node_storage_id); found != instances.end()) {
            return found->second;
        } else {
            NamespaceClass inst = instances[node_storage_id];

            node_storage.register_dependent_asset_cleaner(
                    [instances = &instances, node_storage_id = node_storage_id]() {
                        instances->erase(node_storage_id);
                    });
        }
        return instances[node_storage.id().value];
    }
};
}  // namespace rdf4cpp::rdf::namespaces::util
#endif  //RDF4CPP_NAMESPACEINSTANCE_HPP
