#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {


class IRIBackend {
    std::string iri;
    size_t hash_;

public:
    using View = view::IRIBackendView;
    explicit IRIBackend(std::string_view iri) noexcept;
    explicit IRIBackend(view::IRIBackendView view) noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    explicit operator View() const noexcept;

    [[nodiscard]] size_t hash() const noexcept { return hash_; }
};
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_IRIBACKEND_HPP
