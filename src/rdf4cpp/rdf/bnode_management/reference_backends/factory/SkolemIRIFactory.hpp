#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_FACTORY_IRINODEFACTORY_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_FACTORY_IRINODEFACTORY_HPP

#include <rdf4cpp/rdf/bnode_management/INodeFactory.hpp>

#include <string>

namespace rdf4cpp::rdf::util {

/**
 * A factory that created Skolem IRIs.
 * The resulting IRIs are not explicitly scoped in the node storage even if a scope is provided.
 */
struct SkolemIRIFactory final : INodeFactory {
private:
    std::string iri_prefix;

public:
    explicit SkolemIRIFactory(std::string iri_prefix) noexcept;

    [[nodiscard]] storage::node::identifier::NodeBackendHandle make_node(IIdGenerator &generator,
                                                                         NodeScope const *scope,
                                                                         storage::node::NodeStorage &node_storage) noexcept override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_REFERENCEBACKENDS_FACTORY_IRINODEFACTORY_HPP
