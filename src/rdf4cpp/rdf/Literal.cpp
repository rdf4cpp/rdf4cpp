#include "Literal.hpp"

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/LiteralBackend.hpp>

namespace rdf4cpp::rdf {

Literal::Literal(const Node::NodeID &id) : Node(id) {}
Literal::Literal() : Node() {}
Literal::Literal(const std::string &lexical_form, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_string_literal(lexical_form).second}) {}
Literal::Literal(const std::string &lexical_form, const IRI &datatype, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{
              node_storage.get_typed_literal(
                                  lexical_form,
                                  datatype.handle_.id())
                      .second}) {}
Literal::Literal(const std::string &lexical_form, const std::string &lang, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_lang_literal(lexical_form, lang).second}) {}


IRI Literal::datatype() const {
    NodeID datatype_id = handle_.literal_backend().datatype_id();
    return IRI(datatype_id);
}

const std::string &Literal::lexical_form() const {
    return handle_.literal_backend().lexical_form();
}

const std::string &Literal::language_tag() const {
    return handle_.literal_backend().language_tag();
}
Literal::operator std::string() const {
    // TODO: escape non-standard chars correctly
    const auto &literal = handle_.literal_backend();
    if (literal.datatype_id().node_id() == NodeID::rdf_langstring_iri.first) {
        return literal.quote_lexical() + "@" + literal.language_tag();
    } else {
        return literal.quote_lexical() + "^^" + NodeStorage::lookup_iri(literal.datatype_id())->n_string();
    }
}
bool Literal::is_literal() const { return true; }
bool Literal::is_variable() const { return false; }
bool Literal::is_blank_node() const { return false; }
bool Literal::is_iri() const { return false; }

std::strong_ordering Literal::operator<=>(const Literal &other) const {
    auto type = handle_.literal_backend().datatype_id().type();
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
    if (this->handle_ == other.handle_) {
        return true;
    } else if (this->handle_.literal_backend().datatype_id() != other.handle_.literal_backend().datatype_id()) {
        return false;
    } else {
        if(this->handle_.type() == RDFNodeType::Literal) return this->handle_.literal_backend() == other.handle_.literal_backend();
        else return false;
    }
}

Literal::Literal(Node::BackendNodeHandle handle) : Node(handle) {}
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