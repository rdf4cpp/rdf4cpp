#include "BackendNodeHandle.hpp"

namespace rdf4cpp::rdf::storage::node {

BackendNodeHandle::BackendNodeHandle(NodeID node_id) : node_id_(node_id) {
}

RDFNodeType BackendNodeHandle::type() const noexcept {
    return node_id_.type();
}
bool BackendNodeHandle::is_iri() const noexcept {
    return type() == RDFNodeType::IRI;
}
bool BackendNodeHandle::is_literal() const noexcept {
    return type() == RDFNodeType::Literal;
}
bool BackendNodeHandle::is_blank_node() const noexcept {
    return type() == RDFNodeType::BNode;
}
bool BackendNodeHandle::is_variable() const noexcept {
    return type() == RDFNodeType::Variable;
}
bool BackendNodeHandle::empty() const noexcept {
    return node_id_.empty();
}
NodeID BackendNodeHandle::id() const noexcept {
    return node_id_;
}
IRIBackendHandle BackendNodeHandle::iri_backend() const {
    return NodeStorage::get_iri_handle(node_id_);
}
LiteralBackendHandle BackendNodeHandle::literal_backend() const {
    return NodeStorage::get_literal_handle(node_id_);
}
BNodeBackendHandle BackendNodeHandle::bnode_backend() const {
    return NodeStorage::get_bnode_handle(node_id_);
}
VariableBackendHandle BackendNodeHandle::variable_backend() const {
    return NodeStorage::get_variable_handle(node_id_);
}
NodeStorage BackendNodeHandle::node_storage() const {
    return *NodeStorage::lookup_instance(node_id_.manager_id());
}

}  // namespace rdf4cpp::rdf::storage::node