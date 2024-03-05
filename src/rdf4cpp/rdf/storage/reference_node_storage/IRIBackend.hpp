#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include "rdf4cpp/rdf/storage/view/IRIBackendView.hpp"
#include "rdf4cpp/rdf/storage/reference_node_storage/detail/ConstString.hpp"

namespace rdf4cpp::rdf::storage::reference_node_storage {

struct IRIBackend {
    using view_type = view::IRIBackendView;
    using id_type = identifier::NodeID;

    size_t hash;
    detail::ConstString identifier;

    explicit IRIBackend(view_type const &view) noexcept : hash{view.hash()},
                                                          identifier{view.identifier} {
    }

    explicit operator view_type() const noexcept {
        return view_type{.identifier = identifier};
    }
};

}  // namespace rdf4cpp::rdf::storage::reference_node_storage

#endif  //RDF4CPP_IRIBACKEND_HPP
