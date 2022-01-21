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
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] NodeStorage node_storage() const;

    [[nodiscard]] NodeID id() const noexcept;

    [[nodiscard]] IRIBackendHandle iri_backend() const;
    [[nodiscard]] LiteralBackendHandle literal_backend() const;
    [[nodiscard]] BNodeBackendHandle bnode_backend() const;
    [[nodiscard]] VariableBackendHandle variable_backend() const;

    bool operator==(const BackendNodeHandle &other) const = default;

    auto operator<=>(const BackendNodeHandle &other) const = default;

    // TODO: support value retrieval from basic XSD datatypes
};
}  // namespace rdf4cpp::rdf::storage::node

template<>
struct std::hash<rdf4cpp::rdf::storage::node::BackendNodeHandle> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::BackendNodeHandle const &v) const noexcept {
        return v.id().raw();
    }
};

#endif  //RDF4CPP_BACKENDNODEHANDLE_HPP
