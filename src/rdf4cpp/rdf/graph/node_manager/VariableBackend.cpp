#include "VariableBackend.h"
namespace rdf4cpp::rdf::graph::node_manager {

VariableBackend::VariableBackend(std::string name, bool anonymous) : name(std::move(name)), anonymous(anonymous) {}
std::weak_ordering VariableBackend::operator<=>(const VariableBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::weak_ordering::greater;
}
std::string VariableBackend::as_string([[maybe_unused]] bool quoting) const {
    if (anonymous)
        return "_:" + name;
    else
        return "?" + name;
}
}  // namespace rdf4cpp::rdf::graph::node_manager