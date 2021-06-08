#include "IRIBackend.h"
namespace rdf4cpp::rdf::graph::node_manager {

IRIBackend::IRIBackend(std::string iri) : iri(std::move(iri)) {}
std::weak_ordering IRIBackend::operator<=>(const IRIBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::weak_ordering::greater;
}
std::string IRIBackend::as_string([[maybe_unused]] bool quoting) const {
    // TODO: fix impl.
    return iri;
}
}  // namespace rdf4cpp::rdf::graph::node_manager