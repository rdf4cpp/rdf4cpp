#ifndef RDF4CPP_REFERENCEBLANKNODEIDSCOPE_HPP
#define RDF4CPP_REFERENCEBLANKNODEIDSCOPE_HPP

#include <rdf4cpp/rdf/BlankNode.hpp>
#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/util/IBlankNodeScopeBackend.hpp>
#include <rdf4cpp/rdf/util/BlankNodeIdGenerator.hpp>

#include <memory>
#include <shared_mutex>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct ReferenceBlankNodeIdScope : IBlankNodeScopeBackend {
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

    map_type<std::unique_ptr<ReferenceBlankNodeIdScope>> subscopes; // uses unique_ptr to pin subscopes in memory so that they don't move on map realloc

    void forward_mapping(std::string_view label, Handle hnd);
public:
    explicit ReferenceBlankNodeIdScope(generator_type &generator);
    ReferenceBlankNodeIdScope(ReferenceBlankNodeIdScope &&other) noexcept;

    [[nodiscard]] BlankNode generate_bnode(NodeStorage &node_storage) override;
    [[nodiscard]] IRI generate_skolem_iri(std::string_view iri_prefix, NodeStorage &node_storage) override;

    [[nodiscard]] BlankNode try_get_bnode(std::string_view label) const noexcept override;
    [[nodiscard]] IRI try_get_skolem_iri(std::string_view label) const noexcept override;

    [[nodiscard]] BlankNode get_or_generate_bnode(std::string_view label, NodeStorage &node_storage) override;
    [[nodiscard]] IRI get_or_generate_skolem_iri(std::string_view iri_prefix, std::string_view label, NodeStorage &node_storage) override;

    [[nodiscard]] ReferenceBlankNodeIdScope &subscope(std::string scope_name) noexcept override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_REFERENCEBLANKNODEIDSCOPE_HPP
