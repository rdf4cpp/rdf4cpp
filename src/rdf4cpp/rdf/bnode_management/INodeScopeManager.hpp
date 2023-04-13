#ifndef RDF4CPP_INODESCOPEMANAGER_HPP
#define RDF4CPP_INODESCOPEMANAGER_HPP

#include <string_view>

namespace rdf4cpp::rdf::util {

struct NodeScope;

struct INodeScopeManager {
    using node_scope_type = util::NodeScope;
    [[nodiscard]] virtual node_scope_type &subscope(std::string_view subnamespace) noexcept = 0;
};

} // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_INODESCOPEMANAGER_HPP
