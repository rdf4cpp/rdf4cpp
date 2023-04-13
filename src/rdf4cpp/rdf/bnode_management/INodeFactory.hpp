#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE
#define RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE

#include <rdf4cpp/rdf/bnode_management/IIdGenerator.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

namespace rdf4cpp::rdf::util {

struct NodeScope;

/**
 * A factory for nodes. Any kind of node. That given a generator and scope
 * produces a node that somehow incorporates the generated id of the generator and maybe the scope.
 */
struct INodeFactory {
    virtual ~INodeFactory() noexcept = default;

    [[nodiscard]] virtual storage::node::identifier::NodeBackendHandle make_node(IIdGenerator &generator,
                                                                                 NodeScope const *scope,
                                                                                 storage::node::NodeStorage &node_storage) noexcept = 0;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE
