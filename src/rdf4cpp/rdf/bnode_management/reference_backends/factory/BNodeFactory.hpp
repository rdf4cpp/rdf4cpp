#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP

#include "rdf4cpp/rdf/bnode_management/INodeFactory.hpp"

namespace rdf4cpp::rdf::util {

/**
 * A factory that created BlankNodes.
 * The resulting BlankNodes are scoped to the given scope.
 */
struct BNodeFactory final : INodeFactory {
    static BNodeFactory &default_instance() noexcept;

    [[nodiscard]] storage::node::identifier::NodeBackendHandle make_node(IIdGenerator &generator,
                                                                         NodeScope const *scope,
                                                                         storage::node::NodeStorage &node_storage) noexcept override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP
