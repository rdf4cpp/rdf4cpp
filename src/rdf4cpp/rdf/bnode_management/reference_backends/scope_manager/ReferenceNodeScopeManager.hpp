#ifndef RDF4CPP_REFERENCENODESCOPEMANAGER_HPP
#define RDF4CPP_REFERENCENODESCOPEMANAGER_HPP

#include <rdf4cpp/rdf/bnode_management/INodeScopeManager.hpp>
#include <dice/sparse-map/sparse_map.hpp>

#include <dice/hash.hpp>

namespace rdf4cpp::rdf::util {

struct ReferenceNodeScopeManager : INodeScopeManager {
    using node_scope_type = util::NodeScope;
    using node_scope_storage_type = dice::sparse_map::sparse_map<
            std::string,
            node_scope_type,
            dice::hash::DiceHashwyhash<std::string_view>,
            std::equal_to<>>;

private:
    ReferenceNodeScopeManager() noexcept = default;

public:
    [[nodiscard]] static ReferenceNodeScopeManager &default_instance() noexcept;
    [[nodiscard]] static ReferenceNodeScopeManager new_instance() noexcept;

    node_scope_storage_type scopes;
    node_scope_type &scope(std::string_view name) noexcept override;
};

} // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_REFERENCENODESCOPEMANAGER_HPP
