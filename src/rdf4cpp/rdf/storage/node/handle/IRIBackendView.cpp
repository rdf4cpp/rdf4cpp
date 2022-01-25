#include "IRIBackendView.hpp"
namespace rdf4cpp::rdf::storage::node::handle {
std::string IRIBackendView::n_string() const noexcept {
    return "<" + std::string{identifier} + ">";
}
}  // namespace rdf4cpp::rdf::storage::node::handle