#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE
#define RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE

#include <rdf4cpp/bnode_mngt/IIdGenerator.hpp>
#include <rdf4cpp/storage/NodeStorage.hpp>
#include <rdf4cpp/storage/identifier/NodeBackendHandle.hpp>


namespace rdf4cpp::bnode_mngt {

struct NodeScope;

/**
 * A factory for nodes; so not just BlankNodes. That given a generator and scope
 * produces a node that somehow incorporates the generated id of the generator and maybe the scope.
 */
struct INodeFactory {
    virtual ~INodeFactory() noexcept = default;

    [[nodiscard]] virtual storage::identifier::NodeBackendHandle make_node(IIdGenerator &generator,
                                                                                 NodeScope const *scope,
                                                                                 storage::DynNodeStorage node_storage) noexcept = 0;
};

}  //namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE
