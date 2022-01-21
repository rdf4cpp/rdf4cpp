#include "IRIBackend.hpp"

namespace rdf4cpp::rdf::storage::node {

IRIBackend::IRIBackend(std::string_view iri) noexcept : iri(iri) {}
std::strong_ordering IRIBackend::operator<=>(std::unique_ptr<IRIBackend> const &other) const noexcept {
    if (other)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}
std::string IRIBackend::n_string() const noexcept {
    return "<" + iri + ">";
}
std::string_view IRIBackend::identifier() const noexcept {
    return iri;
}
std::strong_ordering operator<=>(const std::unique_ptr<IRIBackend> &self, const std::unique_ptr<IRIBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node