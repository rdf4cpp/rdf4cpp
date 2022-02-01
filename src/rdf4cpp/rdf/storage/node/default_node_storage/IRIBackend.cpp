#include "IRIBackend.hpp"

namespace rdf4cpp::rdf::storage::node::default_node_storage {

IRIBackend::IRIBackend(std::string_view iri) noexcept : iri(iri) {}
IRIBackend::IRIBackend(handle::IRIBackendView view) noexcept : iri(view.identifier) {}
std::partial_ordering IRIBackend::operator<=>(std::unique_ptr<IRIBackend> const &other) const noexcept {
    if (other)
        return *this <=> *other;
    else
        return std::partial_ordering::greater;
}
std::string_view IRIBackend::identifier() const noexcept {
    return iri;
}
IRIBackend::operator handle::IRIBackendView() const noexcept {
    return {.identifier = identifier()};
}
std::partial_ordering operator<=>(const std::unique_ptr<IRIBackend> &self, const std::unique_ptr<IRIBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage