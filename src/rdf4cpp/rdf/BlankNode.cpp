#include "BlankNode.hpp"

namespace rdf4cpp::rdf {
BlankNode::BlankNode(const Node::NodeID &id) : Node(id) {}
BlankNode::BlankNode() : Node{} {}
BlankNode::BlankNode(const std::string &identifier, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_bnode(identifier).second}) {}
BlankNode::BlankNode(Node::BackendNodeHandle handle) : Node(handle) {}

std::string_view BlankNode::identifier() const { return handle_.bnode_backend().identifier(); }

BlankNode::operator std::string() const {
    return handle_.bnode_backend().n_string();
}

bool BlankNode::is_literal() const { return false; }
bool BlankNode::is_variable() const { return false; }
bool BlankNode::is_blank_node() const { return true; }
bool BlankNode::is_iri() const { return false; }
std::ostream &operator<<(std::ostream &os, const BlankNode &node) {
    os << (std::string) node;
    return os;
}

}  // namespace rdf4cpp::rdf