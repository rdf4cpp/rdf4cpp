#ifndef RDF4CPP_RDFNODE_H
#define RDF4CPP_RDFNODE_H

#include <optional>
#include <string>

#include <rdf4cpp/rdf/graph/node_storage/BackendNodeHandle.h>

namespace rdf4cpp::rdf::node {
class Literal;
class BlankNode;
class IRIResource;
class Variable;

/**
 * The abstract RDFNode class, containing either a Literal, Variable, IRIResource or BlankNode
 */
class RDFNode {
protected:
    using BackendNodeHandle = rdf4cpp::rdf::graph::node_storage::BackendNodeHandle;
    using NodeID = rdf4cpp::rdf::graph::node_storage::NodeID;
    using NodeManager = rdf4cpp::rdf::graph::node_storage::NodeStorage;
    using RDFNodeType = rdf4cpp::rdf::graph::node_storage::RDFNodeType;
    BackendNodeHandle handle_;

    explicit RDFNode(NodeID id);
    explicit RDFNode(const BackendNodeHandle &id);

public:
    RDFNode() = default;

    [[nodiscard]] std::string as_string(bool quoting = false) const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    bool operator==(const RDFNode &other) const;

    std::strong_ordering operator<=>(const RDFNode &other) const;

    explicit operator BlankNode() const;
    explicit operator IRIResource() const;
    explicit operator Literal() const;
    explicit operator Variable() const;

    // todo empty() / unbound()
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_RDFNODE_H
