#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP

#include <rdf4cpp/bnode_mngt/INodeFactory.hpp>

namespace rdf4cpp::bnode_mngt {

/**
 * A factory that created BlankNodes.
 * The resulting BlankNodes are scoped to the given scope.
 */
struct BNodeFactory final : INodeFactory {
    static BNodeFactory &default_instance() noexcept;

    [[nodiscard]] storage::identifier::NodeBackendHandle make_node(IIdGenerator &generator,
                                                                   NodeScope const *scope,
                                                                   storage::DynNodeStoragePtr node_storage) noexcept override;
};

}  //namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP
