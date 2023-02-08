#include "BlankNode.hpp"

#include <rdf4cpp/rdf/bnode_management/INodeScope.hpp>

namespace rdf4cpp::rdf {

BlankNode::BlankNode() noexcept : Node{NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::BNode, {}}} {}
BlankNode::BlankNode(std::string_view identifier, Node::NodeStorage &node_storage) : Node{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier, .scope = nullptr}),
                                                                                                            storage::node::identifier::RDFNodeType::BNode,
                                                                                                            node_storage.id()}} {}
BlankNode::BlankNode(Node::NodeBackendHandle handle) noexcept : Node(handle) {}

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

BlankNode BlankNode::to_node_storage(Node::NodeStorage &node_storage) const noexcept {
    if (this->backend_handle().node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto const node_id = node_storage.find_or_make_id(NodeStorage::find_bnode_backend_view(this->backend_handle()));
    return BlankNode{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::BNode, node_storage.id()}};
}

bool BlankNode::merge_eq(BlankNode const &other) const noexcept {
    if (this->handle_ == other.handle_) {
        return true;
    }

    auto const this_backend = this->handle_.bnode_backend();
    auto const other_backend = other.handle_.bnode_backend();

    return this_backend.identifier == other_backend.identifier && this_backend.scope == other_backend.scope;
}
util::TriBool BlankNode::union_eq(BlankNode const &other) const noexcept {
    if (this->handle_ == other.handle_) {
        return true;
    }

    auto const this_backend = this->handle_.bnode_backend();
    auto const other_backend = other.handle_.bnode_backend();

    if (this_backend.scope.null() || other_backend.scope.null()) {
        return this_backend.scope.null() == other_backend.scope.null() && this_backend.identifier == other_backend.identifier;
    }

    auto this_scope = this_backend.scope.try_upgrade();
    if (!this_scope.has_value()) {
        return util::TriBool::Err;
    }

    auto other_scope = other_backend.scope.try_upgrade();
    if (!other_scope.has_value()) {
        return util::TriBool::Err;
    }

    auto const this_label = (*this_scope)->find_label(this->handle_);
    assert(this_label.has_value());

    auto const other_label = (*other_scope)->find_label(other.handle_);
    assert(other_label.has_value());

    return *this_label == *other_label;
}
}  // namespace rdf4cpp::rdf