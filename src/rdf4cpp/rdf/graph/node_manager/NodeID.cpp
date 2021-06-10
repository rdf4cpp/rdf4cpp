#include "rdf4cpp/rdf/graph/node_manager/NodeID.h"
namespace rdf4cpp::rdf::graph::node_manager {

const std::pair<NodeID, std::string> NodeID::xsd_string_iri = {NodeID(1, RDFNodeType::IRI), "http://www.w3.org/2001/XMLSchema#string"};
const std::pair<NodeID, std::string> NodeID::xsd_langstring_iri = {NodeID(2, RDFNodeType::IRI), "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};

const std::vector<std::pair<NodeID, std::string>> NodeID::predefined_iris = {
        NodeID::xsd_string_iri,
        NodeID::xsd_langstring_iri};

const size_t NodeID::min_iri_id = NodeID(NodeID::predefined_iris.size() + 1, RDFNodeType::IRI).id();
const size_t NodeID::min_literal_id = NodeID(1, RDFNodeType::Literal).id();
const size_t NodeID::min_bnode_id = NodeID(1, RDFNodeType::BNode).id();
const size_t NodeID::min_variable_id = NodeID(1, RDFNodeType::Variable).id();

size_t NodeID::add_type_tag(size_t id, RDFNodeType node_type) {
    std::bitset<size_t_bits> id_bs{id};
    const std::bitset<2> node_type_bs{static_cast<uint8_t>(node_type)};
    id_bs[size_t_bits - 2] = node_type_bs[0];
    id_bs[size_t_bits - 1] = node_type_bs[1];
    return id_bs.to_ulong();
}
NodeID::NodeID(size_t id) : id_(id) {}
NodeID::NodeID(size_t id, RDFNodeType node_type) : id_(add_type_tag(id, node_type)) {}
size_t NodeID::id() const {
    return id_;
}
constexpr RDFNodeType NodeID::type() const {
    return static_cast<RDFNodeType>(uint8_t(id_ >> (size_t_bits - 2)));
}
constexpr bool NodeID::is_type(RDFNodeType type_) const {
    return type_ == type();
}
constexpr bool NodeID::empty() const {
    return id_ == size_t(0);
}
}  // namespace rdf4cpp::rdf::graph::node_manager