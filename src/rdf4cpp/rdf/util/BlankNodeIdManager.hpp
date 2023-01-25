#ifndef RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP

#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

#include <string>
#include <string_view>
#include <memory>
#include <shared_mutex>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator {
private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    explicit BlankNodeIdGenerator(uint64_t seed);
public:
    BlankNodeIdGenerator(BlankNodeIdGenerator &&other) noexcept;

    [[nodiscard]] static BlankNodeIdGenerator from_entropy();
    [[nodiscard]] static BlankNodeIdGenerator from_seed(uint64_t seed);

    ~BlankNodeIdGenerator() noexcept;

    void reseed(uint64_t seed);
    [[nodiscard]] std::string generate_id();
};

struct BlankNodeIdManager {
    using NodeStorage = storage::node::NodeStorage;
    using label_mapping_type = storage::util::tsl::sparse_map<std::string,
                                                              storage::node::identifier::NodeBackendHandle,
                                                              storage::util::robin_hood::hash<std::string_view>,
                                                              std::equal_to<>>;

    using generator_type = BlankNodeIdGenerator;

private:
    std::shared_mutex mutable mutex;
    generator_type generator;
    label_mapping_type label_to_storage;
    NodeStorage node_storage;

    BlankNodeIdManager(generator_type &&generator, label_mapping_type mapping_state, NodeStorage &node_storage);
public:
    [[nodiscard]] static BlankNodeIdManager &default_instance();
    [[nodiscard]] static BlankNodeIdManager from_entropy(NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] static BlankNodeIdManager from_seed(uint64_t seed, NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] static BlankNodeIdManager from_seed_with_state(uint64_t seed, label_mapping_type mapping_state, NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] BlankNode generate();
    [[nodiscard]] BlankNode try_get(std::string_view label) const noexcept;
    [[nodiscard]] BlankNode get_or_generate(std::string_view label);
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
