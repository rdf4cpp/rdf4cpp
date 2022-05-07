#include "VariableBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

VariableBackend::VariableBackend(std::string_view name, bool anonymous) noexcept
    : name_(name),
      anonymous_(anonymous),
      hash_(View(*this).hash()) {}
VariableBackend::VariableBackend(view::VariableBackendView view) noexcept
    : name_(view.name),
      anonymous_(view.is_anonymous),
      hash_(View(*this).hash()) {}
bool VariableBackend::is_anonymous() const noexcept {
    return anonymous_;
}
std::string_view VariableBackend::name() const noexcept {
    return name_;
}
VariableBackend::operator view::VariableBackendView() const noexcept {
    return {.name = name(),
            .is_anonymous = is_anonymous()};
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage