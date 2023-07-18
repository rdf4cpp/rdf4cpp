#include "BNodeBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

BNodeBackend::BNodeBackend(std::string_view identifier) noexcept
    : identifier_(identifier),
      hash_(View(*this).hash()) {}
BNodeBackend::BNodeBackend(view::BNodeBackendView view) noexcept
    : identifier_(view.identifier),
      hash_(View(*this).hash()) {}
std::string_view BNodeBackend::identifier() const noexcept {
    return identifier_;
}
BNodeBackend::operator View() const noexcept {
    return View{.identifier = identifier()};
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage