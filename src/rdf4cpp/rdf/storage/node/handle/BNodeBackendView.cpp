#include "BNodeBackendView.hpp"
namespace rdf4cpp::rdf::storage::node::handle {
std::string BNodeBackendView::n_string() const noexcept {
    return "_:" + std::string{identifier};
}
}  // namespace rdf4cpp::rdf::storage::node::handle