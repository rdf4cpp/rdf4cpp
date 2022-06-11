#include "BlankNode.hpp"

namespace rdf4cpp::rdf {
BlankNode::BlankNode() noexcept : Node{} {}
BlankNode::BlankNode(std::string_view identifier, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier}),
                             storage::node::identifier::RDFNodeType::BNode,
                             node_storage.id()}) {}
BlankNode::BlankNode(Node::NodeBackendHandle handle) noexcept : Node(handle) {}

std::string_view BlankNode::identifier() const { return handle_.bnode_backend().identifier; }

BlankNode::operator std::string() const {
    return "_:" + std::string{handle_.bnode_backend().identifier};
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