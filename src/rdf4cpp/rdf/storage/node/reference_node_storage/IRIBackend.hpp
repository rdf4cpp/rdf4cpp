#ifndef RDF4CPP_IRIBACKEND_HPP
#define RDF4CPP_IRIBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>

#include <string>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct IRIBackend {
    using View = view::IRIBackendView;

    size_t hash;
    std::string identifier;

    explicit IRIBackend(View const &view) noexcept : hash{view.hash()},
                                                     identifier{view.identifier} {
    }

    explicit operator View() const noexcept {
        return View{.identifier = identifier};
    }
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_IRIBACKEND_HPP
