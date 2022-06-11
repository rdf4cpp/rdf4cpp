#include "Variable.hpp"

namespace rdf4cpp::rdf::query {
Variable::Variable() noexcept : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Variable, {}}) {}
Variable::Variable(std::string_view identifier, bool anonymous, NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::VariableBackendView{.name = identifier, .is_anonymous = anonymous}),
                             storage::node::identifier::RDFNodeType::Variable,
                             node_storage.id()}) {}
Variable::Variable(Node::NodeBackendHandle handle) noexcept : Node(handle) {}

bool Variable::is_anonymous() const {
    // TODO: encode is_anonymous into variable ID
    return this->handle_.variable_backend().is_anonymous;
}
std::string_view Variable::name() const {
    return this->handle_.variable_backend().name;
}
Variable::operator std::string() const {
    return handle_.variable_backend().n_string();
}
bool Variable::is_literal() const { return false; }
bool Variable::is_variable() const { return true; }
bool Variable::is_blank_node() const { return false; }
bool Variable::is_iri() const { return false; }
Node::RDFNodeType Variable::type() const { return RDFNodeType::Variable; }
std::ostream &operator<<(std::ostream &os, const Variable &variable) {
    os << (std::string) variable;
    return os;
}

}  // namespace rdf4cpp::rdf::query