#ifndef RDF4CPP_REFERENCENODESCOPEMANAGER_HPP
#define RDF4CPP_REFERENCENODESCOPEMANAGER_HPP

#include <rdf4cpp/rdf/bnode_management/INodeScopeManager.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::util {

struct ReferenceNodeScopeManager : INodeScopeManager {
    using node_scope_type = util::NodeScope;
    using node_scope_storage_type = rdf4cpp::rdf::storage::util::tsl::sparse_map<
            std::string,
            node_scope_type,
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>,
            std::equal_to<>>;

    [[nodiscard]] static ReferenceNodeScopeManager &default_instance() noexcept;
    [[nodiscard]] static ReferenceNodeScopeManager new_instance() noexcept;

    node_scope_storage_type subscopes;
    node_scope_type &subscope(std::string_view subnamespace) noexcept override;
};

} // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_REFERENCENODESCOPEMANAGER_HPP
