#ifndef RDF4CPP_NODEBACKENDHANDLE_HPP
#define RDF4CPP_NODEBACKENDHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>


#include <compare>
#include <cstddef>

namespace rdf4cpp::rdf::storage::node::handle {

class NodeBackendHandle {
    identifier::NodeID node_id_;

public:
    NodeBackendHandle() = default;
    explicit NodeBackendHandle(identifier::NodeID node_id);

    [[nodiscard]] identifier::RDFNodeType type() const noexcept;

    [[nodiscard]] bool is_iri() const noexcept;
    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] NodeStorage node_storage() const;

    [[nodiscard]] identifier::NodeID id() const noexcept;

    [[nodiscard]] IRIBackendView iri_backend() const;
    [[nodiscard]] LiteralBackendView literal_backend() const;
    [[nodiscard]] BNodeBackendView bnode_backend() const;
    [[nodiscard]] VariableBackendView variable_backend() const;

    bool operator==(const NodeBackendHandle &other) const = default;

    auto operator<=>(const NodeBackendHandle &other) const = default;
};
}  // namespace rdf4cpp::rdf::storage::node::handle

template<>
struct std::hash<rdf4cpp::rdf::storage::node::handle::NodeBackendHandle> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::handle::NodeBackendHandle const &v) const noexcept {
        return v.id().raw();
    }
};

#endif  //RDF4CPP_NODEBACKENDHANDLE_HPP
