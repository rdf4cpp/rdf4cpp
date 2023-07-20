#ifndef RDF4CPP_NAMESPACEINSTANCE_HPP
#define RDF4CPP_NAMESPACEINSTANCE_HPP

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

#include <dice/sparse-map/sparse_map.hpp>
#include <dice/hash.hpp>

#include <cinttypes>
#include <mutex>

namespace rdf4cpp::rdf::namespaces::util {

template<typename NamespaceClass>
struct NamespaceInstance {
    static NamespaceClass &instance(storage::node::NodeStorage &node_storage = storage::node::NodeStorage::default_instance()) {
        static std::mutex instances_mutex;
        static dice::sparse_map::sparse_map<uint16_t, NamespaceClass, dice::hash::DiceHashwyhash<uint16_t>> instances;

        uint16_t const node_storage_id = node_storage.id().value;
        std::unique_lock lock{instances_mutex};

        if (auto found = instances.find(node_storage_id); found != instances.end()) {
            if (!found->second.node_storage().try_upgrade().has_value()) {
                instances.erase(found);
                auto [it, inserted] = instances.emplace(node_storage_id, node_storage);
                assert(inserted);
                return it.value();
            }

            return found.value();
        }

        auto [it, inserted] = instances.emplace(node_storage_id, node_storage);
        return it.value();
    }
};
}  // namespace rdf4cpp::rdf::namespaces::util
#endif  //RDF4CPP_NAMESPACEINSTANCE_HPP
