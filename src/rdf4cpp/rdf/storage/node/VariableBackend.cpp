#include "VariableBackend.hpp"

namespace rdf4cpp::rdf::storage::node {

VariableBackend::VariableBackend(std::string name, bool anonymous) : name_(std::move(name)), anonymous_(anonymous) {}
std::strong_ordering VariableBackend::operator<=>(const VariableBackend *other) const {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}
std::string VariableBackend::as_string([[maybe_unused]] bool quoting) const {
    if (anonymous_)
        return "_:" + name_;
    else
        return "?" + name_;
}
bool VariableBackend::is_anonymous() const {
    return anonymous_;
}
const std::string &VariableBackend::name() const {
    return name_;
}
}  // namespace rdf4cpp::rdf::storage::node