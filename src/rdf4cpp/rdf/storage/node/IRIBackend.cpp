#include "IRIBackend.hpp"

namespace rdf4cpp::rdf::storage::node {

IRIBackend::IRIBackend(std::string iri) : iri(std::move(iri)) {}
std::strong_ordering IRIBackend::operator<=>(const IRIBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}
std::string IRIBackend::n_string() const {
    return "<" + iri + ">";
}
std::string IRIBackend::identifier() const {
    return iri;
}
}  // namespace rdf4cpp::rdf::storage::node