#include "NodeBackendHandle.hpp"

namespace rdf4cpp::rdf::storage::node::handle {

NodeBackendHandle::NodeBackendHandle(identifier::NodeID node_id) noexcept : node_id_(node_id) {
}

identifier::RDFNodeType NodeBackendHandle::type() const noexcept {
    return node_id_.type();
}
bool NodeBackendHandle::is_iri() const noexcept {
    return type() == identifier::RDFNodeType::IRI;
}
bool NodeBackendHandle::is_literal() const noexcept {
    return type() == identifier::RDFNodeType::Literal;
}
bool NodeBackendHandle::is_blank_node() const noexcept {
    return type() == identifier::RDFNodeType::BNode;
}
bool NodeBackendHandle::is_variable() const noexcept {
    return type() == identifier::RDFNodeType::Variable;
}
bool NodeBackendHandle::empty() const noexcept {
    return node_id_.empty();
}
identifier::NodeID const &NodeBackendHandle::id() const noexcept {
    return node_id_;
}
IRIBackendView NodeBackendHandle::iri_backend() const noexcept {
    return NodeStorage::get_iri_handle(node_id_);
}
LiteralBackendView NodeBackendHandle::literal_backend() const noexcept {
    return NodeStorage::get_literal_handle(node_id_);
}
BNodeBackendView NodeBackendHandle::bnode_backend() const noexcept {
    return NodeStorage::get_bnode_handle(node_id_);
}
VariableBackendView NodeBackendHandle::variable_backend() const noexcept {
    return NodeStorage::get_variable_handle(node_id_);
}
NodeStorage NodeBackendHandle::node_storage() const noexcept {
    return *NodeStorage::lookup_instance(node_id_.manager_id());
}

}  // namespace rdf4cpp::rdf::storage::node::handle