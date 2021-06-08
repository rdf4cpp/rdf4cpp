
#include "BNodeBackend.h"

namespace rdf4cpp::rdf::graph::node_manager {

BNodeBackend::BNodeBackend(std::string identifier) : identifier(std::move(identifier)) {}
std::weak_ordering BNodeBackend::operator<=>(const BNodeBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::weak_ordering::greater;
}
std::string BNodeBackend::as_string([[maybe_unused]] bool quoting) const {
    return "_:" + identifier;
}
}  // namespace rdf4cpp::rdf::graph::node_manager