#ifndef RDF4CPP_REFERENCENODESCOPEMANAGER_HPP
#define RDF4CPP_REFERENCENODESCOPEMANAGER_HPP

#include <rdf4cpp/bnode_mngt/INodeScopeManager.hpp>
#include <dice/sparse-map/sparse_map.hpp>

#include <dice/hash.hpp>

namespace rdf4cpp::bnode_mngt {

struct ReferenceNodeScopeManager : INodeScopeManager {
    using node_scope_storage_type = dice::sparse_map::sparse_map<
            std::string,
            NodeScope,
            dice::hash::DiceHashwyhash<std::string_view>,
            std::equal_to<>>;

    static ReferenceNodeScopeManager &default_instance() noexcept;

    node_scope_storage_type scopes;
    NodeScope &scope(std::string_view name) noexcept override;
};

} // namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_REFERENCENODESCOPEMANAGER_HPP
