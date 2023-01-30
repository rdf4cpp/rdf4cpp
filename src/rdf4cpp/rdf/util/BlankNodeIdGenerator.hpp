#ifndef RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP

#include <rdf4cpp/rdf/util/BlankNodeIdScope.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator {
    using NodeStorage = storage::node::NodeStorage;
private:
    struct Impl;
    std::unique_ptr<Impl> impl;

    explicit BlankNodeIdGenerator(std::unique_ptr<Impl> &&impl);
public:
    BlankNodeIdGenerator(BlankNodeIdGenerator &&other) noexcept;
    ~BlankNodeIdGenerator() noexcept;

    [[nodiscard]] static BlankNodeIdGenerator &default_instance();
    [[nodiscard]] static BlankNodeIdGenerator from_entropy();
    [[nodiscard]] static BlankNodeIdGenerator from_seed(uint64_t seed);

    static size_t generated_id_length() noexcept;

    [[nodiscard]] std::string generate_id();
    [[nodiscard]] BlankNode generated_bnode(NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] IRI generate_skolem_iri(std::string_view iri_prefix, NodeStorage &node_storage = NodeStorage::default_instance());

    BlankNodeIdScope scope(NodeStorage &node_storage = NodeStorage::default_instance());
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
