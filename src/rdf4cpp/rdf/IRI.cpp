#include "IRI.hpp"

namespace rdf4cpp::rdf {

IRI::IRI(const Node::NodeID &id) : Node(id) {}
IRI::IRI(Node::BackendNodeHandle handle) : Node(handle) {}
IRI::IRI() : Node() {}
IRI::IRI(std::string_view iri, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_iri_id(iri)}) {}

IRI::operator std::string() const { return handle_.iri_backend().n_string(); }

bool IRI::is_literal() const { return false; }
bool IRI::is_variable() const { return false; }
bool IRI::is_blank_node() const { return false; }
bool IRI::is_iri() const { return true; }


IRI IRI::default_graph(NodeStorage &node_storage) {
    return IRI("", node_storage);
}
std::ostream &operator<<(std::ostream &os, const IRI &iri) {
    os << (std::string) iri;
    return os;
}
std::string_view IRI::identifier() const {
    return handle_.iri_backend().identifier;
}

}  // namespace rdf4cpp::rdf