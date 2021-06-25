#include "IRI.hpp"

namespace rdf4cpp::rdf {

IRI::IRI(const Node::NodeID &id) : Node(id) {}
IRI::IRI(Node::BackendNodeHandle handle) : Node(handle) {}
IRI::IRI() : Node() {}
IRI::IRI(const std::string &iri, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_iri(iri).second}) {}

std::string IRI::as_string(bool quoting) const { return handle_.iri_backend().as_string(quoting); }

bool IRI::is_blank_node() const { return false; }
bool IRI::is_literal() const { return false; }
bool IRI::is_variable() const { return false; }
bool IRI::is_bnode() const { return false; }
bool IRI::is_iri() const { return true; }
Node::RDFNodeType IRI::type() const { return RDFNodeType::IRI; }


IRI IRI::default_graph(NodeStorage &node_storage) {
    return IRI("", node_storage);
}

}  // namespace rdf4cpp::rdf