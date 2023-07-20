#ifndef RDF4CPP_BNODEBACKEND_HPP
#define RDF4CPP_BNODEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>

#include <string>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct BNodeBackend {
    using View = view::BNodeBackendView;

    size_t hash;
    std::string identifier;

    explicit BNodeBackend(View const &view) noexcept : hash{view.hash()},
                                                       identifier{view.identifier} {
    }

    explicit operator View() const noexcept {
        return View{.identifier = identifier};
    }
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_BNODEBACKEND_HPP
