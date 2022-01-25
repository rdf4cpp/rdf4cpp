#include "VariableBackendView.hpp"
namespace rdf4cpp::rdf::storage::node::handle {
std::string VariableBackendView::n_string() const noexcept {
    if (is_anonymous)
        return "_:" + std::string{name};
    else
        return "?" + std::string{name};
}
}  // namespace rdf4cpp::rdf::storage::node::handle