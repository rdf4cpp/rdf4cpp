#include "rdf4cpp/rdf/graph/node_manager/VariableBackend.h"
namespace rdf4cpp::rdf::graph::node_manager {

VariableBackend::VariableBackend(std::string name, bool anonymous) : name_(std::move(name)), anonymous_(anonymous) {}
std::weak_ordering VariableBackend::operator<=>(const VariableBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::weak_ordering::greater;
}
std::string VariableBackend::as_string([[maybe_unused]] bool quoting) const {
    if (anonymous_)
        return "_:" + name_;
    else
        return "?" + name_;
}
}  // namespace rdf4cpp::rdf::graph::node_manager