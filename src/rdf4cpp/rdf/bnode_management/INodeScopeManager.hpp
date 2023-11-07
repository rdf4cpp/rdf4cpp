#ifndef RDF4CPP_INODESCOPEMANAGER_HPP
#define RDF4CPP_INODESCOPEMANAGER_HPP

#include <string_view>

namespace rdf4cpp::rdf::util {

struct NodeScope;

/**
 * Manages a set of scopes, by name.
 * I.e. you call subscope with a given name, if there is no scope
 * referring to that name a new one is created. Otherwise you get the previously created scope.
 *
 * You can think of this as a map<std::string, NodeScope>.
 */
struct INodeScopeManager {
    using node_scope_type = util::NodeScope;
    [[nodiscard]] virtual node_scope_type &scope(std::string_view name) noexcept = 0;
};

} // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_INODESCOPEMANAGER_HPP
