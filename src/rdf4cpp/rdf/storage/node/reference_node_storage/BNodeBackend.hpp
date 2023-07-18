#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

class BNodeBackend {
    std::string identifier_;
    size_t hash_;

public:
    using View = view::BNodeBackendView;
    explicit BNodeBackend(std::string_view identifier) noexcept;
    explicit BNodeBackend(view::BNodeBackendView view) noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    [[nodiscard]] size_t hash() const noexcept { return hash_; }

    explicit operator View() const noexcept;
};
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_BNODEBACKEND_HPP
