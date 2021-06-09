#include "rdf4cpp/rdf/graph/node_manager/TaggedResourcePointer.h"
namespace rdf4cpp::rdf::graph::node_manager {
std::bitset<TaggedResourcePtr::size_t_bits> TaggedResourcePtr::encode_ptr(const void *ptr, ID id) {
    std::bitset<size_t_bits> ptr_bits{size_t(ptr)};
    const std::bitset<size_t_bits> id_bits{id.id()};
    ptr_bits[size_t_bits - 1] = id_bits[size_t_bits - 1];
    ptr_bits[size_t_bits - 2] = id_bits[size_t_bits - 2];
    return ptr_bits;
}
void *TaggedResourcePtr::ptr() const {
    std::bitset<size_t_bits> bits_ = this->bits;
    bits_[size_t_bits - 1] = false;
    bits_[size_t_bits - 2] = false;
    return reinterpret_cast<void *>(bits_.to_ulong());
}
TaggedResourcePtr::TaggedResourcePtr(void *ptr, ID id) : bits(encode_ptr(ptr, id)) {}
RDFNodeType TaggedResourcePtr::type() const {
    return static_cast<RDFNodeType>(uint8_t(bits.to_ulong() >> (size_t_bits - 2)));
}
bool TaggedResourcePtr::is_iri() const {
    return type() == RDFNodeType::IRI;
}
bool TaggedResourcePtr::is_literal() const {
    return type() == RDFNodeType::Literal;
}
bool TaggedResourcePtr::is_bnode() const {
    return type() == RDFNodeType::BNode;
}
bool TaggedResourcePtr::is_variable() const {
    return type() == RDFNodeType::Variable;
}
bool TaggedResourcePtr::empty() const {
    return bits.none();
}
ID TaggedResourcePtr::id(ResourceManager &node_manager) {
    return node_manager.lookup_id(ptr(), type());
}
IRIBackend &TaggedResourcePtr::iri() const {
    return *static_cast<IRIBackend *>(ptr());
}
LiteralBackend &TaggedResourcePtr::literal() const {
    return *static_cast<LiteralBackend *>(ptr());
}
BNodeBackend &TaggedResourcePtr::bnode() const {
    return *static_cast<BNodeBackend *>(ptr());
}
VariableBackend &TaggedResourcePtr::variable() const {
    return *static_cast<VariableBackend *>(ptr());
}
}  // namespace rdf4cpp::rdf::graph::node_manager