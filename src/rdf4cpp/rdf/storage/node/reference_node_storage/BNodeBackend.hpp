#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/bnode_management/NodeScope.hpp>

#include <compare>
#include <memory>
#include <string>
#include <string_view>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

class BNodeBackend {
    std::string identifier_;
    std::optional<rdf4cpp::rdf::util::WeakNodeScope> scope_;
    size_t hash_;

public:
    using View = view::BNodeBackendView;
    explicit BNodeBackend(std::string_view identifier, std::optional<rdf4cpp::rdf::util::WeakNodeScope> scope) noexcept;
    explicit BNodeBackend(view::BNodeBackendView view) noexcept;

    [[nodiscard]] std::string_view identifier() const noexcept;

    [[nodiscard]] size_t hash() const noexcept { return hash_; }

    explicit operator view::BNodeBackendView() const noexcept;
};
}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_BNODEBACKEND_HPP
