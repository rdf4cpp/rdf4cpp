#include "IRIBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

IRIBackend::IRIBackend(std::string_view iri) noexcept
    : iri(iri),
      hash_(View(*this).hash()) {}
IRIBackend::IRIBackend(view::IRIBackendView view) noexcept
    : iri(view.identifier),
      hash_(View(*this).hash()) {}
std::string_view IRIBackend::identifier() const noexcept {
    return iri;
}
IRIBackend::operator View() const noexcept {
    return View{.identifier = identifier()};
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage