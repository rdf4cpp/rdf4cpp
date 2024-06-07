#ifndef RDF4CPP_SINGLESCOPEMANAGER_HPP
#define RDF4CPP_SINGLESCOPEMANAGER_HPP

#include <rdf4cpp/bnode_mngt/NodeScopeManager.hpp>

namespace rdf4cpp::bnode_mngt {

/**
 * A scope manager that always gives you the same scope.
 * Primarily useful for using union-semantics during parsing.
 */
template<NodeScope S = ReferenceNodeScope<>> requires std::is_default_constructible_v<S>
struct UnionNodeScopeManager {
    S scope_;

    S &scope([[maybe_unused]] std::string_view name) noexcept {
        return scope_;
    }
};
static_assert(NodeScopeManager<UnionNodeScopeManager<>>);

} // namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_SINGLESCOPEMANAGER_HPP
