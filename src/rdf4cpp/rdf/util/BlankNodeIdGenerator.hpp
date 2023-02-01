#ifndef RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP

#include <rdf4cpp/rdf/util/BlankNodeIdScope.hpp>
#include <rdf4cpp/rdf/util/IBlankNodeIdGeneratorBackend.hpp>
#include <rdf4cpp/rdf/util/reference_backends/RandomIdGeneratorBackend.hpp>
#include <rdf4cpp/rdf/util/reference_backends/IncreasingIdGeneratorBackend.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct BlankNodeIdGenerator {
    using NodeStorage = storage::node::NodeStorage;
private:
    std::unique_ptr<IBlankNodeIdGeneratorBackend> impl;

    explicit BlankNodeIdGenerator(std::unique_ptr<IBlankNodeIdGeneratorBackend> &&impl);
public:
    BlankNodeIdGenerator(BlankNodeIdGenerator &&other) noexcept;
    ~BlankNodeIdGenerator() noexcept;

    [[nodiscard]] static BlankNodeIdGenerator &default_instance();
    [[nodiscard]] static BlankNodeIdGenerator with_backend(std::unique_ptr<IBlankNodeIdGeneratorBackend> backend);
    [[nodiscard]] static BlankNodeIdGenerator from_entropy();
    [[nodiscard]] static BlankNodeIdGenerator from_seed(uint64_t seed);

    [[nodiscard]] size_t max_generated_id_size() const noexcept;

    [[nodiscard]] std::string generate_id();
    [[nodiscard]] BlankNode generated_bnode(NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] IRI generate_skolem_iri(std::string_view iri_prefix, NodeStorage &node_storage = NodeStorage::default_instance());

    BlankNodeIdScope scope();

    template<typename Backend> requires std::derived_from<IBlankNodeScopeBackend, Backend>
    BlankNodeIdScope scope() {
        return BlankNodeIdScope{std::make_unique<Backend>(*this)};
    }
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
