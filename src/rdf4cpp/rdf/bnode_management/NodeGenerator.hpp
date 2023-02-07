#ifndef RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP

#include <rdf4cpp/rdf/bnode_management/IIdGenerator.hpp>
#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/factory/SkolemIRIFactory.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/factory/UnscopedBNodeFactory.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/generator/IncreasingIdGenerator.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/generator/RandomIdGenerator.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::util {

struct NodeGenerator {
    using NodeStorage = storage::node::NodeStorage;
private:
    std::unique_ptr<IIdGenerator> impl;
    explicit NodeGenerator(std::unique_ptr<IIdGenerator> &&impl);
public:
    NodeGenerator(NodeGenerator &&other) noexcept = default;

    [[nodiscard]] static NodeGenerator &default_instance();
    [[nodiscard]] static NodeGenerator with_backend(std::unique_ptr<IIdGenerator> backend);

    template<typename GeneratorBackend, typename ...Args> requires std::derived_from<GeneratorBackend, IIdGenerator>
    static NodeGenerator with_backend(Args &&...args) {
        return NodeGenerator{std::make_unique<GeneratorBackend>(std::forward<Args>(args)...)};
    }

    [[nodiscard]] std::string generate_id();
    [[nodiscard]] Node generate_node(INodeFactory &facade = UnscopedBNodeFactory::default_instance(), NodeStorage &node_storage = NodeStorage::default_instance());

    NodeScope scope();

    template<typename Backend, typename ...Args> requires std::derived_from<Backend, INodeScope>
    NodeScope scope(Args &&...args) {
        return NodeScope{this->impl.get(), SharedPtr<Backend>::make(std::forward<Args>(args)...)};
    }
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEMANAGER_HPP
