#include <rdf4cpp/rdf/bnode_management/reference_backends/scope_manager/ReferenceNodeScopeManager.hpp>

#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>

namespace rdf4cpp::rdf::util {

ReferenceNodeScopeManager &ReferenceNodeScopeManager::default_instance() noexcept {
    static ReferenceNodeScopeManager instance_;
    return instance_;
}

NodeScope &ReferenceNodeScopeManager::scope(std::string_view name) noexcept {
    if (auto it = scopes.find(name); it != scopes.end()) {
        return it.value();
    }

    auto [it, inserted] = scopes.emplace(name, util::NodeScope::new_instance());
    assert(inserted);
    return it.value();
}

} // namespace rdf4cpp::rdf::util
