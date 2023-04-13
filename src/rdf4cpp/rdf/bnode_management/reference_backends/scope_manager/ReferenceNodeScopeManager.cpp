#include <rdf4cpp/rdf/bnode_management/reference_backends/scope_manager/ReferenceNodeScopeManager.hpp>

#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>

namespace rdf4cpp::rdf::util {

ReferenceNodeScopeManager::node_scope_type &ReferenceNodeScopeManager::subscope(std::string_view subnamespace) noexcept {
    if (auto it = subscopes.find(subnamespace); it != subscopes.end()) {
        return it.value();
    }

    auto [it, inserted] = subscopes.emplace(subnamespace, util::NodeScope::new_instance());
    assert(inserted);
    return it.value();
}

ReferenceNodeScopeManager &ReferenceNodeScopeManager::default_instance() noexcept {
    static ReferenceNodeScopeManager instance_;
    return instance_;
}

ReferenceNodeScopeManager ReferenceNodeScopeManager::new_instance() noexcept {
    return ReferenceNodeScopeManager{};
}

} // namespace rdf4cpp::rdf::util
