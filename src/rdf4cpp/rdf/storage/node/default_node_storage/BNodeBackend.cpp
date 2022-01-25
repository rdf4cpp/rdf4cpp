#include "BNodeBackend.hpp"

namespace rdf4cpp::rdf::storage::node::default_node_storage {

BNodeBackend::BNodeBackend(std::string_view identifier) noexcept
    : identifier_(identifier) {}
std::strong_ordering BNodeBackend::operator<=>(std::unique_ptr<BNodeBackend> const &other) const noexcept {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}
std::string_view BNodeBackend::identifier() const noexcept {
    return identifier_;
}
BNodeBackend::operator handle::BNodeBackendView() const noexcept {
    return {.identifier = identifier()};
}
std::strong_ordering operator<=>(const std::unique_ptr<BNodeBackend> &self, const std::unique_ptr<BNodeBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node::default_node_storage