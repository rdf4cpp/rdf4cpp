#include "BNodeBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

BNodeBackend::BNodeBackend(std::string_view identifier, rdf4cpp::rdf::util::WeakPtr<rdf4cpp::rdf::util::INodeScope const> scope) noexcept
    : identifier_(identifier),
      scope_{std::move(scope)},
      hash_(View(*this).hash()) {}
BNodeBackend::BNodeBackend(view::BNodeBackendView view) noexcept
    : identifier_(view.identifier),
      scope_{view.scope},
      hash_(View(*this).hash()) {}
std::string_view BNodeBackend::identifier() const noexcept {
    return identifier_;
}
BNodeBackend::operator view::BNodeBackendView() const noexcept {
    return {.identifier = identifier(), .scope = this->scope_};
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage