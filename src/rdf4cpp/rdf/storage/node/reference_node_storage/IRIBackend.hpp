#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct IRIBackend {
    using View = view::IRIBackendView;
    using Id = identifier::NodeID;

    size_t hash;
    detail::ConstString identifier;

    explicit IRIBackend(View const &view) noexcept : hash{view.hash()},
                                                     identifier{view.identifier} {
    }

    explicit operator View() const noexcept {
        return View{.identifier = identifier};
    }
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_IRIBACKEND_HPP
