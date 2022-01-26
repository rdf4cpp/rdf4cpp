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
    NodeBackendHandle() noexcept = default;
    explicit NodeBackendHandle(identifier::NodeID node_id) noexcept;

    [[nodiscard]] identifier::RDFNodeType type() const noexcept;

    [[nodiscard]] bool is_iri() const noexcept;
    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] NodeStorage node_storage() const noexcept;

    [[nodiscard]] identifier::NodeID const &id() const noexcept;

    [[nodiscard]] IRIBackendView iri_backend() const noexcept;
    [[nodiscard]] LiteralBackendView literal_backend() const noexcept;
    [[nodiscard]] BNodeBackendView bnode_backend() const noexcept;
    [[nodiscard]] VariableBackendView variable_backend() const noexcept;

    bool operator==(const NodeBackendHandle &other) const noexcept = default;

    auto operator<=>(const NodeBackendHandle &other) const noexcept = default;
};
}  // namespace rdf4cpp::rdf::storage::node::handle

template<>
struct std::hash<rdf4cpp::rdf::storage::node::handle::NodeBackendHandle> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::handle::NodeBackendHandle const &v) const noexcept {
        return v.id().raw();
    }
};

#endif  //RDF4CPP_NODEBACKENDHANDLE_HPP
