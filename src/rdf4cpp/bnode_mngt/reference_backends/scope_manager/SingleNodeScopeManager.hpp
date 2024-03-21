#ifndef RDF4CPP_SINGLESCOPEMANAGER_HPP
#define RDF4CPP_SINGLESCOPEMANAGER_HPP

#include <rdf4cpp/bnode_mngt/NodeScope.hpp>
#include <rdf4cpp/bnode_mngt/INodeScopeManager.hpp>

namespace rdf4cpp::bnode_mngt {

/**
 * A scope manager that always gives you the same scope.
 * Primarily useful for using union-semantics during parsing.
 */
struct SingleNodeScopeManager : INodeScopeManager {
    using node_scope_storage_type = NodeScope;

    node_scope_storage_type scope_;

    NodeScope &scope([[maybe_unused]] std::string_view name) noexcept override {
        return scope_;
    }
};

} // namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_SINGLESCOPEMANAGER_HPP
