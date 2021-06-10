#include "Variable.h"

namespace rdf4cpp::rdf::node {
Variable::Variable() : RDFNode() {}
Variable::Variable(void *ptr, const RDFNode::NodeID &id) : RDFNode(ptr, id) {}
Variable::Variable(const std::string &identifier, bool anonymous, NodeManager &node_manager)
    : RDFNode(BackendNodeHandle{node_manager.get_variable(identifier, anonymous).first, RDFNodeType::Variable}) {}
Variable::Variable(RDFNode::BackendNodeHandle handle) : RDFNode(handle) {}

bool Variable::is_anonymous() const {
    return this->handle_.variable_backend().is_anonymous();
}
const std::string &Variable::name() const {
    return this->handle_.variable_backend().name();
}
std::string Variable::as_string(bool quoting) const {
    return handle_.variable_backend().as_string(quoting);
}
bool Variable::is_blank_node() const { return false; }
bool Variable::is_literal() const { return false; }
bool Variable::is_variable() const { return true; }
bool Variable::is_bnode() const { return false; }
bool Variable::is_iri() const { return false; }
RDFNode::RDFNodeType Variable::type() const { return RDFNodeType::Variable; }

}  // namespace rdf4cpp::rdf::node