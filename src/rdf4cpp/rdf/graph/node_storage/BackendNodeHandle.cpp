#include "BackendNodeHandle.h"

namespace rdf4cpp::rdf::graph::node_storage {

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
bool BackendNodeHandle::is_bnode() const noexcept {
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
IRIBackend &BackendNodeHandle::iri_backend() const {
    return *NodeStorage::lookup_iri(node_id_);
}
LiteralBackend &BackendNodeHandle::literal_backend() const {
    return *NodeStorage::lookup_literal(node_id_);
}
BNodeBackend &BackendNodeHandle::bnode_backend() const {
    return *NodeStorage::lookup_bnode(node_id_);
}
VariableBackend &BackendNodeHandle::variable_backend() const {
    return *NodeStorage::lookup_variable(node_id_);
}
NodeStorage BackendNodeHandle::node_context() const {
    return *NodeStorage::lookup_instance(node_id_.manager_id());
}

}  // namespace rdf4cpp::rdf::graph::node_storage