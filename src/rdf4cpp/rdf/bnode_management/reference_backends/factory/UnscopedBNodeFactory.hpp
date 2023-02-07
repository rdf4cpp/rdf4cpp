#ifndef RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP
#define RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP

#include "rdf4cpp/rdf/bnode_management/INodeFactory.hpp"

namespace rdf4cpp::rdf::util {

struct UnscopedBNodeFactory : INodeFactory {
    static UnscopedBNodeFactory &default_instance() noexcept;

    [[nodiscard]] storage::node::identifier::NodeBackendHandle make_null_node() const noexcept override;
    [[nodiscard]] storage::node::identifier::NodeBackendHandle make_node(IIdGenerator &generator, storage::node::NodeStorage &node_storage) override;
};

}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_BNODEMANAGEMENT_UNSCOPEDBNODEFACADE_HPP
