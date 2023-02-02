#include "BlankNode.hpp"

namespace rdf4cpp::rdf {
BlankNode::BlankNode() noexcept : Node{NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::BNode, {}}} {}
BlankNode::BlankNode(std::string_view identifier, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier}),
                             storage::node::identifier::RDFNodeType::BNode,
                             node_storage.id()}) {}
BlankNode::BlankNode(Node::NodeBackendHandle handle) noexcept : Node(handle) {}

BlankNode BlankNode::make_null() noexcept {
    return BlankNode{};
}

BlankNode BlankNode::make(std::string_view identifier, Node::NodeStorage &node_storage) {
    return BlankNode{identifier, node_storage};
}

BlankNode BlankNode::to_node_storage(Node::NodeStorage &node_storage) const noexcept {
    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto const node_id = node_storage.find_or_make_id(NodeStorage::find_bnode_backend_view(handle_));
    return BlankNode{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::BNode, node_storage.id()}};
}

std::string_view BlankNode::identifier() const noexcept { return handle_.bnode_backend().identifier; }

BlankNode::operator std::string() const noexcept {
    return "_:" + std::string{handle_.bnode_backend().identifier};
}

bool BlankNode::is_literal() const noexcept { return false; }
bool BlankNode::is_variable() const noexcept { return false; }
bool BlankNode::is_blank_node() const noexcept { return true; }
bool BlankNode::is_iri() const noexcept { return false; }
std::ostream &operator<<(std::ostream &os, const BlankNode &node) {
    os << static_cast<std::string>(node);
    return os;
}

}  // namespace rdf4cpp::rdf