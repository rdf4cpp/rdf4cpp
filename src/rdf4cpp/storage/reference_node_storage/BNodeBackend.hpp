#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/bnode_mngt/NodeScope.hpp>
#include <rdf4cpp/storage/view/BNodeBackendView.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::rdf::storage::reference_node_storage {

struct BNodeBackend {
    using view_type = view::BNodeBackendView;
    using id_type = identifier::NodeID;

    size_t hash;
    detail::ConstString identifier;
    std::optional<rdf4cpp::rdf::bnode_mngt::WeakNodeScope> scope;

    explicit BNodeBackend(view_type const &view) noexcept : hash{view.hash()},
                                                            identifier{view.identifier},
                                                            scope{view.scope} {
    }

    explicit operator view_type() const noexcept {
        return view_type{.identifier = identifier, .scope = scope};
    }
};

}  // namespace rdf4cpp::rdf::storage::reference_node_storage

#endif  //RDF4CPP_BNODEBACKEND_HPP
