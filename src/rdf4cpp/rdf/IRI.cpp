#include "IRI.hpp"

namespace rdf4cpp::rdf {

IRI::IRI(Node::NodeBackendHandle handle) noexcept : Node(handle) {}
IRI::IRI() noexcept : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::IRI, {}}) {}
IRI::IRI(std::string_view iri, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::IRIBackendView{.identifier = iri}),
                             storage::node::identifier::RDFNodeType::IRI,
                             node_storage.id()}) {}

IRI IRI::from_datatype_iri(datatypes::registry::DatatypeIRIView const iri, NodeStorage &node_storage) noexcept {
    if (iri.is_fixed()) {
        return IRI{NodeBackendHandle{NodeID{static_cast<uint64_t>(iri.get_fixed())},
                                     storage::node::identifier::RDFNodeType::IRI,
                                     node_storage.id()}};
    } else {
        return IRI{iri.get_other(), node_storage};
    }
}

datatypes::registry::DatatypeIRIView IRI::to_datatype_iri() const noexcept {
    auto const id = this->handle_.node_id().value();
    storage::node::identifier::LiteralType const type = storage::node::identifier::LiteralType::from_iri_node_id(id);

    if (type.is_fixed()) {
        return datatypes::registry::DatatypeIRIView{type.to_underlying()};
    } else {
        return datatypes::registry::DatatypeIRIView{this->identifier()};
    }
}

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