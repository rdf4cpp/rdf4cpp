#ifndef RDF4CPP_BACKENDNODEHANDLE_H
#define RDF4CPP_BACKENDNODEHANDLE_H

#include <rdf4cpp/rdf/graph/node_manager/NodeManager.h>

#include <bitset>
#include <compare>
#include <cstddef>

namespace rdf4cpp::rdf::graph::node_manager {

class BackendNodeHandle {
    constexpr static size_t size_t_bits = NodeID::size_t_bits;
    std::bitset<size_t_bits> bits{};

    static std::bitset<size_t_bits> encode_ptr(void const *ptr, NodeID id);

    static std::bitset<size_t_bits> encode_ptr(void const *ptr, RDFNodeType type);

    [[nodiscard]] void *ptr() const;

public:
    BackendNodeHandle() = default;
    BackendNodeHandle(void *ptr, NodeID id);
    BackendNodeHandle(void *ptr, RDFNodeType type);

    [[nodiscard]] RDFNodeType type() const;

    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_variable() const;

    [[nodiscard]] bool empty() const;

    NodeID id(NodeManager &node_manager = NodeManager::default_instance());

    [[nodiscard]] IRIBackend &iri_backend() const;
    [[nodiscard]] LiteralBackend &literal_backend() const;
    [[nodiscard]] BNodeBackend &bnode_backend() const;
    [[nodiscard]] VariableBackend &variable_backend() const;

    bool operator==(const BackendNodeHandle &other) const;

    std::strong_ordering operator<=>(const BackendNodeHandle &other) const;
};
}  // namespace rdf4cpp::rdf::graph::node_manager

#endif  //RDF4CPP_BACKENDNODEHANDLE_H
