#include "Variable.hpp"

namespace rdf4cpp::rdf::query {
Variable::Variable() : Node() {}
Variable::Variable(const Node::NodeID &id) : Node(id) {}
Variable::Variable(const std::string &identifier, bool anonymous, NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_variable(identifier, anonymous).second}) {}
Variable::Variable(Node::BackendNodeHandle handle) : Node(handle) {}

bool Variable::is_anonymous() const {
    // TODO: encode is_anonymous into variable ID
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
Node::RDFNodeType Variable::type() const { return RDFNodeType::Variable; }

}  // namespace rdf4cpp::rdf::query