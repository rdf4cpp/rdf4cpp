#include "Literal.hpp"

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/LiteralBackend.hpp>

namespace rdf4cpp::rdf {

Literal::Literal() : Node(NodeBackendHandle{{}, storage::node::identifier::RDFNodeType::Literal, {}}) {}
Literal::Literal(std::string_view lexical_form, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                     .datatype_id = storage::node::identifier::NodeID::xsd_string_iri.first,
                                     .lexical_form = lexical_form,
                                     .language_tag = ""}),
                             storage::node::identifier::RDFNodeType::Literal,
                             node_storage.id()}) {}

Literal::Literal(std::string_view lexical_form, const IRI &datatype, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                     .datatype_id = datatype.to_node_storage(node_storage).backend_handle().node_id(),
                                     .lexical_form = lexical_form,
                                     .language_tag = ""}),
                             storage::node::identifier::RDFNodeType::Literal,
                             node_storage.id()}) {}
Literal::Literal(std::string_view lexical_form, std::string_view lang, Node::NodeStorage &node_storage)
    : Node(NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::LiteralBackendView{
                                     .datatype_id = storage::node::identifier::NodeID::rdf_langstring_iri.first,
                                     .lexical_form = lexical_form,
                                     .language_tag = lang}),
                             storage::node::identifier::RDFNodeType::Literal,
                             node_storage.id()}) {}

IRI Literal::datatype() const {
    NodeBackendHandle iri_handle{handle_.literal_backend().datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()};
    return IRI(iri_handle);
}

std::string_view Literal::lexical_form() const {
    return handle_.literal_backend().lexical_form;
}

std::string_view Literal::language_tag() const {
    return handle_.literal_backend().language_tag;
}
Literal::operator std::string() const {
    // TODO: escape non-standard chars correctly
    auto quote_lexical = [](std::string_view lexical) -> std::string {
        // TODO: escape quotes (") in lexical + escape everything that needs to be escaped in N-Tripels/N-Quads
        return "\"" + std::string{lexical} + "\"";
    };
    const auto &literal = handle_.literal_backend();
    if (literal.datatype_id == NodeID::rdf_langstring_iri.first) {
        return quote_lexical(literal.lexical_form) + "@" + std::string{literal.language_tag};
    } else {
        auto const &dtype_iri = NodeStorage::find_iri_backend_view(NodeBackendHandle{literal.datatype_id, storage::node::identifier::RDFNodeType::IRI, backend_handle().node_storage_id()});
        return quote_lexical(literal.lexical_form) + "^^" + dtype_iri.n_string();
    }
}
bool Literal::is_literal() const { return true; }
bool Literal::is_variable() const { return false; }
bool Literal::is_blank_node() const { return false; }
bool Literal::is_iri() const { return false; }
Literal::Literal(Node::NodeBackendHandle handle) : Node(handle) {}

std::partial_ordering Literal::operator<=>(const Literal &other) const {
    auto type = handle_.type();
    if (auto comp_id = this->handle_ <=> other.handle_; comp_id == std::partial_ordering::equivalent) {
        return std::strong_ordering::equal;
    } else if (auto comp_type = this->handle_.type() <=> other.handle_.type(); comp_type != std::strong_ordering::equal) {
        return comp_type;
    } else {  // same type, different id.
        switch (this->handle_.type()) {
            case RDFNodeType::IRI:
                return this->handle_.iri_backend() <=> other.handle_.iri_backend();
            case RDFNodeType::BNode:
                return this->handle_.bnode_backend() <=> other.handle_.bnode_backend();
            case RDFNodeType::Literal:
                return this->handle_.literal_backend() <=> other.handle_.literal_backend();
            case RDFNodeType::Variable:
                return this->handle_.variable_backend() <=> other.handle_.variable_backend();
        }
        return std::strong_ordering::less;
    }
}
bool Literal::operator==(const Literal &other) const {
    if (this->lexical_form() == other.lexical_form()) {
        return true;
    } else if (this->datatype() != other.datatype()) {
        return false;
    } else {
        return false;
        /*if(this->handle_.type() == RDFNodeType::Literal) return this->handle_.literal_backend() == other.handle_.literal_backend();
        else return false;*/
    }
}

std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << (std::string) literal;
    return os;
}
std::any Literal::value() const {
    datatypes::DatatypeRegistry::factory_fptr_t factory = datatypes::DatatypeRegistry::get_factory(this->datatype().identifier());
    if (factory != nullptr)
        return factory(lexical_form());
    else
        return {};
}


}  // namespace rdf4cpp::rdf