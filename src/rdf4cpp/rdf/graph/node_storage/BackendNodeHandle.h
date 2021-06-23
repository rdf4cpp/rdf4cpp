#ifndef RDF4CPP_BACKENDNODEHANDLE_H
#define RDF4CPP_BACKENDNODEHANDLE_H

#include <rdf4cpp/rdf/graph/node_storage/NodeStorage.h>

#include <compare>
#include <cstddef>

namespace rdf4cpp::rdf::graph::node_storage {

class BackendNodeHandle {
    NodeID node_id_;

public:
    BackendNodeHandle() = default;
    BackendNodeHandle(NodeID node_id);

    [[nodiscard]] RDFNodeType type() const noexcept;

    [[nodiscard]] bool is_iri() const noexcept;
    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_bnode() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] NodeStorage node_context() const;

    NodeID id() const noexcept;

    [[nodiscard]] IRIBackend &iri_backend() const;
    [[nodiscard]] LiteralBackend &literal_backend() const;
    [[nodiscard]] BNodeBackend &bnode_backend() const;
    [[nodiscard]] VariableBackend &variable_backend() const;

    bool operator==(const BackendNodeHandle &other) const = default;

    auto operator<=>(const BackendNodeHandle &other) const = default;
};
}  // namespace rdf4cpp::rdf::graph::node_storage

#endif  //RDF4CPP_BACKENDNODEHANDLE_H
