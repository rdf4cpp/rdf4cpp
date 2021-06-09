//
// Created by Lixi Alié Conrads on 5/30/21.
//

#include "BlankNode.h"
namespace rdf4cpp::rdf::node {
BlankNode::BlankNode() : RDFNode{} {}

std::string BlankNode::as_string(bool quoting) const {
    return id_.bnode().as_string(quoting);
}
bool BlankNode::is_blank_node() const { return true; }
BlankNode::BlankNode(void *ptr, const RDFNode::ID &id) : RDFNode(ptr, id) {}


}  // namespace rdf4cpp::rdf::node