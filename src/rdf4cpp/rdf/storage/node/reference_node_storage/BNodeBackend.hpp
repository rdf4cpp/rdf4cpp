#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/bnode_mngt/NodeScope.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct BNodeBackend {
    using View = view::BNodeBackendView;
    using Id = identifier::NodeID;

    size_t hash;
    ConstString identifier;
    std::optional<rdf4cpp::rdf::bnode_mngt::WeakNodeScope> scope;

    explicit BNodeBackend(View const &view) noexcept : hash{view.hash()},
                                                       identifier{view.identifier},
                                                       scope{view.scope == nullptr ? std::nullopt : std::optional{*view.scope}} {
    }

    explicit operator View() const noexcept {
        return View{.identifier = identifier, .scope = scope.has_value() ? &*scope : nullptr};
    }
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_BNODEBACKEND_HPP
