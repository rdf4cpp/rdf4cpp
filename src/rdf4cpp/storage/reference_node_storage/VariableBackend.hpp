#ifndef RDF4CPP_VARIABLEBACKEND_HPP
#define RDF4CPP_VARIABLEBACKEND_HPP

#include <rdf4cpp/storage/view/VariableBackendView.hpp>
#include <rdf4cpp/storage/reference_node_storage/detail/ConstString.hpp>

namespace rdf4cpp::storage::reference_node_storage {

struct VariableBackend {
    using view_type = view::VariableBackendView;
    using id_type = identifier::NodeID;

    size_t hash;
    detail::ConstString name;
    bool is_anonymous;

    explicit VariableBackend(view_type const &view) noexcept : hash{view.hash()},
                                                               name{view.name},
                                                               is_anonymous{view.is_anonymous} {
    }

    explicit operator view_type() const noexcept {
        return view_type{.name = name,
                         .is_anonymous = is_anonymous};
    }
};

}  // namespace rdf4cpp::storage::reference_node_storage

#endif  //RDF4CPP_VARIABLEBACKEND_HPP
