#include "BackendNodeHandle.h"

namespace rdf4cpp::rdf::graph::node_manager {
std::bitset<BackendNodeHandle::size_t_bits> BackendNodeHandle::encode_ptr(const void *ptr, NodeID id) {
    std::bitset<size_t_bits> ptr_bits{size_t(ptr)};
    const std::bitset<size_t_bits> id_bits{id.id()};
    ptr_bits[size_t_bits - 1] = id_bits[size_t_bits - 1];
    ptr_bits[size_t_bits - 2] = id_bits[size_t_bits - 2];
    return ptr_bits;
}

std::bitset<BackendNodeHandle::size_t_bits> BackendNodeHandle::encode_ptr(const void *ptr, RDFNodeType type) {
    std::bitset<size_t_bits> ptr_bits{size_t(ptr)};
    const std::bitset<8> type_bits{uint8_t(type)};
    ptr_bits[size_t_bits - 1] = type_bits[1];
    ptr_bits[size_t_bits - 2] = type_bits[0];
    return ptr_bits;
}

void *BackendNodeHandle::ptr() const {
    std::bitset<size_t_bits> bits_ = this->bits;
    bits_[size_t_bits - 1] = false;
    bits_[size_t_bits - 2] = false;
    return reinterpret_cast<void *>(bits_.to_ulong());
}
BackendNodeHandle::BackendNodeHandle(void *ptr, NodeID id) : bits(encode_ptr(ptr, id)) {}
BackendNodeHandle::BackendNodeHandle(void *ptr, RDFNodeType type) : bits(encode_ptr(ptr, type)) {}

RDFNodeType BackendNodeHandle::type() const {
    return static_cast<RDFNodeType>(uint8_t(bits.to_ulong() >> (size_t_bits - 2)));
}
bool BackendNodeHandle::is_iri() const {
    return type() == RDFNodeType::IRI;
}
bool BackendNodeHandle::is_literal() const {
    return type() == RDFNodeType::Literal;
}
bool BackendNodeHandle::is_bnode() const {
    return type() == RDFNodeType::BNode;
}
bool BackendNodeHandle::is_variable() const {
    return type() == RDFNodeType::Variable;
}
bool BackendNodeHandle::empty() const {
    return bits.none();
}
NodeID BackendNodeHandle::id(NodeManager &node_manager) {
    return node_manager.lookup_id(ptr(), type());
}
IRIBackend &BackendNodeHandle::iri_backend() const {
    return *static_cast<IRIBackend *>(ptr());
}
LiteralBackend &BackendNodeHandle::literal_backend() const {
    return *static_cast<LiteralBackend *>(ptr());
}
BNodeBackend &BackendNodeHandle::bnode_backend() const {
    return *static_cast<BNodeBackend *>(ptr());
}
VariableBackend &BackendNodeHandle::variable_backend() const {
    return *static_cast<VariableBackend *>(ptr());
}
bool BackendNodeHandle::operator==(const BackendNodeHandle &other) const {
    return this->bits == other.bits;
}
std::strong_ordering BackendNodeHandle::operator<=>(const BackendNodeHandle &other) const {
    return this->bits.to_ulong() <=> other.bits.to_ulong();
}
}  // namespace rdf4cpp::rdf::graph::node_manager