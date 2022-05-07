#include "BNodeBackend.hpp"

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

BNodeBackend::BNodeBackend(std::string_view identifier) noexcept
    : identifier_(identifier),
      hash_(View(*this).hash()) {}
BNodeBackend::BNodeBackend(view::BNodeBackendView view) noexcept
    : identifier_(view.identifier),
      hash_(View(*this).hash()) {}
std::partial_ordering BNodeBackend::operator<=>(std::unique_ptr<BNodeBackend> const &other) const noexcept {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::partial_ordering::greater;
}
std::string_view BNodeBackend::identifier() const noexcept {
    return identifier_;
}
BNodeBackend::operator view::BNodeBackendView() const noexcept {
    return {.identifier = identifier()};
}
std::partial_ordering operator<=>(const std::unique_ptr<BNodeBackend> &self, const std::unique_ptr<BNodeBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage