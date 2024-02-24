#ifndef RDF4CPP_VARIABLEBACKEND_HPP
#define RDF4CPP_VARIABLEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

struct VariableBackend {
    using View = view::VariableBackendView;
    using Id = identifier::NodeID;

    size_t hash;
    ConstString name;
    bool is_anonymous;

    explicit VariableBackend(view::VariableBackendView const &view) noexcept : hash{view.hash()},
                                                                               name{view.name},
                                                                               is_anonymous{view.is_anonymous} {
    }

    explicit operator View() const noexcept {
        return View{.name = name,
                    .is_anonymous = is_anonymous};
    }
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_VARIABLEBACKEND_HPP
