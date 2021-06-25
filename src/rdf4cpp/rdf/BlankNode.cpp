#include "BlankNode.hpp"

namespace rdf4cpp::rdf {
BlankNode::BlankNode(const Node::NodeID &id) : Node(id) {}
BlankNode::BlankNode() : Node{} {}
BlankNode::BlankNode(const std::string &identifier, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_bnode(identifier).second}) {}
BlankNode::BlankNode(Node::BackendNodeHandle handle) : Node(handle) {}

bool BlankNode::is_blank_node() const { return true; }
const std::string &BlankNode::identifier() const { return handle_.bnode_backend().indentifier(); }

std::string BlankNode::as_string(bool quoting) const {
    return handle_.bnode_backend().as_string(quoting);
}

bool BlankNode::is_literal() const { return false; }
bool BlankNode::is_variable() const { return false; }
bool BlankNode::is_bnode() const { return false; }
Node::RDFNodeType BlankNode::type() const { return RDFNodeType::BNode; }
bool BlankNode::is_iri() const { return false; }

}  // namespace rdf4cpp::rdf