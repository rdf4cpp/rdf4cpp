#include "BlankNode.hpp"
#include <rdf4cpp/rdf/bnode_mngt/NodeScope.hpp>

#include <rdf4cpp/rdf/writer/TryWrite.hpp>

namespace rdf4cpp::rdf {
BlankNode::BlankNode() noexcept : Node{NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::BNode, {}}} {}
BlankNode::BlankNode(std::string_view identifier, NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::BNodeBackendView{.identifier = identifier, .scope = std::nullopt}),
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

BlankNode BlankNode::find(std::string_view identifier, const NodeStorage& node_storage) noexcept {
    auto nid = node_storage.find_id(storage::node::view::BNodeBackendView{identifier, std::nullopt});
    if (nid.null())
        return BlankNode{};
    return BlankNode{NodeBackendHandle{nid, storage::node::identifier::RDFNodeType::BNode, node_storage.id()}};
}

std::string_view BlankNode::identifier() const noexcept { return handle_.bnode_backend().identifier; }

bool BlankNode::serialize(writer::BufWriterParts const writer) const noexcept {
    auto const backend = handle_.bnode_backend();

    RDF4CPP_DETAIL_TRY_WRITE_STR("_:");
    RDF4CPP_DETAIL_TRY_WRITE_STR(backend.identifier);
    return true;
}

BlankNode::operator std::string() const noexcept {
    return writer::StringWriter::oneshot([this](auto &w) noexcept {
        return this->serialize(w);
    });
}

bool BlankNode::is_literal() const noexcept { return false; }
bool BlankNode::is_variable() const noexcept { return false; }
bool BlankNode::is_blank_node() const noexcept { return true; }
bool BlankNode::is_iri() const noexcept { return false; }
std::ostream &operator<<(std::ostream &os, BlankNode const &bnode) {
    writer::BufOStreamWriter w{os};
    bnode.serialize(w);
    w.finalize();

    return os;
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

    if (!this_backend.scope.has_value() || !other_backend.scope.has_value()) {
        return this_backend.scope.has_value() == other_backend.scope.has_value() && this_backend.identifier == other_backend.identifier;
    }

    auto this_scope = this_backend.scope->try_upgrade();
    if (!this_scope.has_value()) {
        return util::TriBool::Err;
    }

    auto other_scope = other_backend.scope->try_upgrade();
    if (!other_scope.has_value()) {
        return util::TriBool::Err;
    }

    auto const this_label = this_scope->try_get_label(*this);
    assert(this_label.has_value());

    auto const other_label = other_scope->try_get_label(other);
    assert(other_label.has_value());

    return *this_label == *other_label;
}

inline namespace shorthands {

BlankNode operator""_bnode(char const *str, size_t len) {
    return BlankNode{std::string_view{str, len}};
}

}  // namespace literals
}  // namespace rdf4cpp::rdf