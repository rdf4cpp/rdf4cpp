#include "VariableBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

VariableBackend::VariableBackend(std::string_view name, bool anonymous) noexcept : VariableBackend{View{name, anonymous}} {
}

VariableBackend::VariableBackend(view::VariableBackendView view) noexcept : name_{view.name},
                                                                            is_anonymous_{view.is_anonymous},
                                                                            hash_{view.hash()} {
}

VariableBackend::operator View() const noexcept {
    return View{.name = name_,
                .is_anonymous = is_anonymous_};
}

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage
