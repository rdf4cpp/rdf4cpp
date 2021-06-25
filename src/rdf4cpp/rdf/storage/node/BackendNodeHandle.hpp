#ifndef RDF4CPP_BACKENDNODEHANDLE_HPP
#define RDF4CPP_BACKENDNODEHANDLE_HPP

#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

#include <compare>
#include <cstddef>

namespace rdf4cpp::rdf::storage::node {

class BackendNodeHandle {
    NodeID node_id_;

public:
    BackendNodeHandle() = default;
    explicit BackendNodeHandle(NodeID node_id);

    [[nodiscard]] RDFNodeType type() const noexcept;

    [[nodiscard]] bool is_iri() const noexcept;
    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_bnode() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] NodeStorage node_storage() const;

    [[nodiscard]] NodeID id() const noexcept;

    [[nodiscard]] IRIBackend &iri_backend() const;
    [[nodiscard]] LiteralBackend &literal_backend() const;
    [[nodiscard]] BNodeBackend &bnode_backend() const;
    [[nodiscard]] VariableBackend &variable_backend() const;

    bool operator==(const BackendNodeHandle &other) const = default;

    auto operator<=>(const BackendNodeHandle &other) const = default;
};
}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_BACKENDNODEHANDLE_HPP
