#ifndef RDF4CPP_BLANKNODE_H
#define RDF4CPP_BLANKNODE_H

#include <optional>

#include <rdf4cpp/rdf/node/RDFNode.h>

namespace rdf4cpp::rdf::node {
class BlankNode : public RDFNode {

private:
    BlankNode(void *ptr, const NodeID &id);
    explicit BlankNode(BackendNodeHandle handle);

public:
    BlankNode();
    explicit BlankNode(const std::string &identifier,
                       NodeManager &node_manager = NodeManager::default_instance());

    [[nodiscard]] const std::string &identifier() const;

    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting = false) const;
    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;
    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_BLANKNODE_H
