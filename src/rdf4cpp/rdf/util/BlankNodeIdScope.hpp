#ifndef RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP

#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator;

struct BlankNodeIdScope {
    using generator_type = BlankNodeIdGenerator;
    using NodeStorage = storage::node::NodeStorage;

private:
    friend struct BlankNodeIdGenerator;

    struct Handle {
        storage::node::identifier::NodeBackendHandle iri_handle;
        storage::node::identifier::NodeBackendHandle bnode_handle;
    };

    template<typename V>
    using map_type = storage::util::tsl::sparse_map<std::string,
                                                    V,
                                                    storage::util::robin_hood::hash<std::string_view>,
                                                    std::equal_to<>>;

private:
    std::shared_mutex mutable mutex; // protects only label_to_storage
    map_type<Handle> label_to_storage;

    generator_type *generator;
    NodeStorage node_storage;

    map_type<std::unique_ptr<BlankNodeIdScope>> subscopes; // uses unique_ptr to pin subscopes in memory

public:
    BlankNodeIdScope(generator_type &generator, NodeStorage node_storage);
    BlankNodeIdScope(BlankNodeIdScope &&other) noexcept;

    [[nodiscard]] BlankNode generate_bnode();
    [[nodiscard]] IRI generate_skolem_iri(std::string_view iri_prefix);

    [[nodiscard]] BlankNode try_get_bnode(std::string_view label) const noexcept;
    [[nodiscard]] IRI try_get_skolem_iri(std::string_view label) const noexcept;

    [[nodiscard]] BlankNode get_or_generate_bnode(std::string_view label);
    [[nodiscard]] IRI get_or_generate_skolem_iri(std::string_view iri_prefix, std::string_view label);

    [[nodiscard]] BlankNodeIdScope &subscope(std::string const &scope_name) noexcept;
    [[nodiscard]] BlankNodeIdScope &subscope(std::string &&scope_name) noexcept;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
