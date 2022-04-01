#include "NodeID.hpp"

#include <rdf4cpp/rdf/storage/node/identifier/RDFNodeType.hpp>

namespace rdf4cpp::rdf::storage::node::identifier {

std::pair<NodeID, std::string> const NodeID::default_graph_iri = {NodeID(1), ""};
std::pair<NodeID, std::string> const NodeID::xsd_string_iri = {NodeID(2), "http://www.w3.org/2001/XMLSchema#string"};

std::pair<NodeID, std::string> const NodeID::rdf_langstring_iri = {NodeID(3), "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};

std::vector<std::pair<NodeID, std::string>> const NodeID::predefined_iris = {
        NodeID::default_graph_iri,
        NodeID::xsd_string_iri,
        NodeID::rdf_langstring_iri};
NodeID const NodeID::min_iri_id = NodeID(NodeID::predefined_iris.size() + 1);
LiteralID const NodeID::min_literal_id = LiteralID(1);
NodeID const NodeID::min_bnode_id = NodeID(1);
NodeID const NodeID::min_variable_id = NodeID(1);


}  // namespace rdf4cpp::rdf::storage::node::identifier