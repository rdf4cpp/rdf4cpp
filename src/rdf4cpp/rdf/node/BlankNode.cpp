#include "BlankNode.h"

namespace rdf4cpp::rdf::node {
BlankNode::BlankNode(const RDFNode::NodeID &id) : RDFNode(id) {}
BlankNode::BlankNode() : RDFNode{} {}
BlankNode::BlankNode(const std::string &identifier, RDFNode::NodeManager &node_storage)
    : RDFNode(BackendNodeHandle{node_storage.get_bnode(identifier).second}) {}
BlankNode::BlankNode(RDFNode::BackendNodeHandle handle) : RDFNode(handle) {}

bool BlankNode::is_blank_node() const { return true; }
const std::string &BlankNode::identifier() const { return handle_.bnode_backend().indentifier(); }

std::string BlankNode::as_string(bool quoting) const {
    return handle_.bnode_backend().as_string(quoting);
}

bool BlankNode::is_literal() const { return false; }
bool BlankNode::is_variable() const { return false; }
bool BlankNode::is_bnode() const { return false; }
RDFNode::RDFNodeType BlankNode::type() const { return RDFNodeType::BNode; }
bool BlankNode::is_iri() const { return false; }

}  // namespace rdf4cpp::rdf::node