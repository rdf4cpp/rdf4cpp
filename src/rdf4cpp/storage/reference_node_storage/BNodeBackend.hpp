#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/bnode_mngt/NodeScope.hpp>
#include <rdf4cpp/storage/view/BNodeBackendView.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::storage::reference_node_storage {

struct BNodeBackend {
    using view_type = view::BNodeBackendView;
    using id_type = identifier::NodeID;

    size_t hash;
    detail::ConstString identifier;
    std::optional<rdf4cpp::bnode_mngt::WeakNodeScope> scope;

    explicit BNodeBackend(view_type const &view) noexcept : hash{view.hash()},
                                                            identifier{view.identifier},
                                                            scope{view.scope} {
    }

    explicit operator view_type() const noexcept {
        return view_type{.identifier = identifier, .scope = scope};
    }

    static identifier::NodeBackendID from_storage_id(id_type const id, [[maybe_unused]] view_type const view) noexcept {
        return identifier::NodeBackendID{id, identifier::RDFNodeType::BNode};
    }

    static id_type to_storage_id(identifier::NodeBackendID const id) noexcept {
        return id.node_id();
    }
};

}  // namespace rdf4cpp::storage::reference_node_storage

#endif  //RDF4CPP_BNODEBACKEND_HPP
