#ifndef RDF4CPP_IRIRESOURCE_H
#define RDF4CPP_IRIRESOURCE_H

#include <rdf4cpp/rdf/node/RDFNode.h>

namespace rdf4cpp::rdf::node {

/**
 * IRI Resource node.
 */
class IRIResource : public RDFNode {
public:
    IRIResource(const NodeID &id);
    explicit IRIResource(RDFNode::BackendNodeHandle handle);


public:
    IRIResource();

    explicit IRIResource(const std::string &iri,
                         NodeManager &node_storage = NodeManager::primary_instance());
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting = false) const;

    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;
    friend class RDFNode;
    friend class Literal;

    static IRIResource default_graph(NodeManager &node_storage = NodeManager::primary_instance());
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_IRIRESOURCE_H
