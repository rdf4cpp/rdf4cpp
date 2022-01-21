#include "VariableBackend.hpp"

namespace rdf4cpp::rdf::storage::node {

VariableBackend::VariableBackend(std::string_view name, bool anonymous) noexcept
    : name_(name), anonymous_(anonymous) {}
std::strong_ordering VariableBackend::operator<=>(std::unique_ptr<VariableBackend> const &other) const noexcept {
    if (other != nullptr)
        return *this <=> *other;
    else
        return std::strong_ordering::greater;
}
std::string VariableBackend::n_string() const noexcept {
    if (anonymous_)
        return "_:" + name_;
    else
        return "?" + name_;
}
bool VariableBackend::is_anonymous() const noexcept {
    return anonymous_;
}
std::string_view VariableBackend::name() const noexcept {
    return name_;
}
std::strong_ordering operator<=>(const std::unique_ptr<VariableBackend> &self, const std::unique_ptr<VariableBackend> &other) noexcept {
    return *self <=> *other;
}
}  // namespace rdf4cpp::rdf::storage::node