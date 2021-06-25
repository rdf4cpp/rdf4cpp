#include "NodeID.hpp"

namespace rdf4cpp::rdf::storage::node {
LiteralID::LiteralID(uint64_t value) : value(value) {}
LiteralID &LiteralID::operator++() noexcept {
    value++;
    return *this;
}
LiteralID LiteralID::operator++(int) noexcept {
    LiteralID new_literal_id{*this};
    ++value;
    return new_literal_id;
}


NodeIDValue::NodeIDValue(uint64_t value) : value(value) {}
NodeIDValue::NodeIDValue(LiteralID literalId, LiteralType literalType) : literal_{literalId.value, literalType} {}
LiteralID NodeIDValue::literal_id() const {
    return LiteralID{literal_.literal_id_};
}
LiteralType NodeIDValue::literal_type() const {
    return literal_.literal_type_;
}
std::strong_ordering NodeIDValue::operator<=>(const NodeIDValue &other) const {
    return value <=> other.value;
}
bool NodeIDValue::operator==(const NodeIDValue &other) const {
    return value == other.value;
}
NodeIDValue &NodeIDValue::operator++() noexcept {
    ++literal_.literal_id_;
    return *this;
}
NodeIDValue NodeIDValue::operator++(int) noexcept {
    NodeIDValue new_node_id{*this};
    ++literal_.literal_id_;
    return new_node_id;
}


const std::pair<NodeIDValue, std::string> NodeID::default_graph_iri = {NodeIDValue(1), ""};
const std::pair<NodeIDValue, std::string> NodeID::xsd_string_iri = {NodeIDValue(2), "http://www.w3.org/2001/XMLSchema#string"};
const std::pair<NodeIDValue, std::string> NodeID::rdf_langstring_iri = {NodeIDValue(3), "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};

const std::vector<std::pair<NodeIDValue, std::string>> NodeID::predefined_iris = {
        NodeID::default_graph_iri,
        NodeID::xsd_string_iri,
        NodeID::rdf_langstring_iri};

const NodeIDValue NodeID::min_iri_id = NodeIDValue(NodeID::predefined_iris.size() + 1);
const LiteralID NodeID::min_literal_id = LiteralID(1);
const NodeIDValue NodeID::min_bnode_id = NodeIDValue(1);
const NodeIDValue NodeID::min_variable_id = NodeIDValue(1);


NodeID::NodeID() : raw_(0) {}
NodeID::NodeID(size_t raw) : raw_(raw) {}
NodeID::NodeID(NodeStorageID manager_id, RDFNodeType type, NodeIDValue node_id, uint8_t tagging_bits)
    : fields_{{node_id}, type, manager_id.value, tagging_bits} {}
NodeID::NodeID(NodeStorageID manager_id, RDFNodeType type, LiteralID literal_id, LiteralType literal_type, uint8_t tagging_bits)
    : fields_{{literal_id, literal_type}, type, manager_id.value, tagging_bits} {}
NodeStorageID NodeID::manager_id() const {
    return NodeStorageID{fields_.manager_id_};
}
uint8_t NodeID::free_tagging_bits() const {
    return fields_.free_tagging_bits;
}
void NodeID::free_tagging_bits(uint8_t new_value) {
    assert(new_value < (1 << 4));
    fields_.free_tagging_bits = new_value;
}
const NodeIDValue &NodeID::node_id() const {
    return fields_.node_id_;
}
uint64_t NodeID::raw() const {
    return raw_;
}
RDFNodeType NodeID::type() const {
    return static_cast<RDFNodeType>(fields_.type_);
}
bool NodeID::is_type(RDFNodeType type_) const {
    return type_ == type();
}
bool NodeID::empty() const {
    return fields_.node_id_ == 0;
}
std::partial_ordering NodeID::operator<=>(const NodeID &other) const {
    if (fields_.manager_id_ != other.fields_.manager_id_) {
        return std::partial_ordering::unordered;
    } else {
        return (std::tie(fields_.type_, fields_.node_id_) <=> std::tie(other.fields_.type_, other.fields_.node_id_));
    }
}
bool NodeID::operator==(const NodeID &other) const {
    return std::tie(fields_.manager_id_, fields_.type_, fields_.node_id_) ==
           std::tie(other.fields_.manager_id_, other.fields_.type_, other.fields_.node_id_);
}
std::string NodeID::as_string() const {
    std::string str = "<NodeID ";
    if (is_type(RDFNodeType::Literal))
        str += "literal_type=" + std::to_string((uint16_t) node_id().literal_type()) +
               " literal_id=" + std::to_string(node_id().literal_id().value);
    else
        str += "node_id=" + std::to_string(node_id().value);
    str += " type=" + node::as_string(type());
    str += " manager_id=" + std::to_string(manager_id().value);
    str += " free_tagging_bits=" + std::to_string(free_tagging_bits()) + " >";
    return str;
}


}  // namespace rdf4cpp::rdf::storage::node