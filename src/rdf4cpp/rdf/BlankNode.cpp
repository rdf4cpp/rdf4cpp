#include "BlankNode.hpp"

#include <rdf4cpp/rdf/writer/TryWrite.hpp>

namespace rdf4cpp::rdf {
BlankNode::BlankNode() noexcept : Node{NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::BNode, {}}} {}
BlankNode::BlankNode(std::string_view identifier, NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier}),
                             storage::node::identifier::RDFNodeType::BNode,
                             node_storage.id()}) {}
BlankNode::BlankNode(Node::NodeBackendHandle handle) noexcept : Node(handle) {}

BlankNode BlankNode::make_null() noexcept {
    return BlankNode{};
}

BlankNode BlankNode::make(std::string_view identifier, NodeStorage &node_storage) {
    return BlankNode{identifier, node_storage};
}

BlankNode BlankNode::to_node_storage(NodeStorage &node_storage) const noexcept {
    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto const node_id = node_storage.find_or_make_id(NodeStorage::find_bnode_backend_view(handle_));
    return BlankNode{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::BNode, node_storage.id()}};
}

BlankNode BlankNode::try_get_in_node_storage(NodeStorage const &node_storage) const noexcept {
    if (handle_.node_storage_id() == node_storage.id()) {
        return *this;
    }

    auto const node_id = node_storage.find_id(NodeStorage::find_bnode_backend_view(handle_));
    if (node_id == NodeID{}) {
        return BlankNode{};
    }

    return BlankNode{NodeBackendHandle{node_id, storage::node::identifier::RDFNodeType::BNode, node_storage.id()}};
}

std::string_view BlankNode::identifier() const noexcept { return handle_.bnode_backend().identifier; }

bool BlankNode::serialize(void *const buffer, writer::Cursor &cursor, writer::FlushFunc const flush) const noexcept {
    auto const backend = handle_.bnode_backend();

    RDF4CPP_DETAIL_TRY_WRITE_STR("_:");
    RDF4CPP_DETAIL_TRY_WRITE_STR(backend.identifier);
    return true;
}

BlankNode::operator std::string() const noexcept {
    return handle_.bnode_backend().n_string();
}

bool BlankNode::is_literal() const noexcept { return false; }
bool BlankNode::is_variable() const noexcept { return false; }
bool BlankNode::is_blank_node() const noexcept { return true; }
bool BlankNode::is_iri() const noexcept { return false; }
std::ostream &operator<<(std::ostream &os, const BlankNode &bnode) {
    os << static_cast<std::string>(bnode);
    return os;
}


inline namespace shorthands {

BlankNode operator""_bnode(char const *str, size_t len) {
    return BlankNode{std::string_view{str, len}};
}

}  // namespace literals
}  // namespace rdf4cpp::rdf