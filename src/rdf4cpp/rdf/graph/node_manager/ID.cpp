#include "rdf4cpp/rdf/graph/node_manager/ID.h"
namespace rdf4cpp::rdf::graph::node_manager {

size_t ID::add_type_tag(size_t id, RDFNodeType node_type) {
    std::bitset<size_t_bits> id_bs{id};
    const std::bitset<2> node_type_bs{static_cast<uint8_t>(node_type)};
    id_bs[size_t_bits - 2] = node_type_bs[0];
    id_bs[size_t_bits - 1] = node_type_bs[1];
    return id_bs.to_ulong();
}
ID::ID(size_t id) : id_(id) {}
ID::ID(size_t id, RDFNodeType node_type) : id_(add_type_tag(id, node_type)) {}
size_t ID::id() const {
    return id_;
}
constexpr RDFNodeType ID::type() const {
    return static_cast<RDFNodeType>(uint8_t(id_ >> (size_t_bits - 2)));
}
constexpr bool ID::is_type(RDFNodeType type_) const {
    return type_ == type();
}
}  // namespace rdf4cpp::rdf::graph::node_manager