#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE
#define RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE

#include <rdf4cpp/rdf/bnode_management/IIdGenerator.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

namespace rdf4cpp::rdf::util {

struct INodeFactory {
    virtual ~INodeFactory() = default;

    [[nodiscard]] virtual storage::node::identifier::NodeBackendHandle make_null_node() const noexcept = 0;
    [[nodiscard]] virtual storage::node::identifier::NodeBackendHandle make_node(IIdGenerator &generator, storage::node::NodeStorage &node_storage) = 0;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_INODEFACADE
