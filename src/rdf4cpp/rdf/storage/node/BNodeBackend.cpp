#include "BNodeBackend.hpp"

namespace rdf4cpp::rdf::storage::node {

BNodeBackend::BNodeBackend(std::string identifier) noexcept
    : identifier_(std::move(identifier)) {}
std::strong_ordering BNodeBackend::operator<=>(std::unique_ptr<BNodeBackend> const &other) const noexcept {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}
std::string BNodeBackend::n_string() const noexcept {
    return "_:" + identifier_;
}
const std::string &BNodeBackend::indentifier() const noexcept {
    return identifier_;
}
std::strong_ordering operator<=>(const std::unique_ptr<BNodeBackend> &self, const std::unique_ptr<BNodeBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node