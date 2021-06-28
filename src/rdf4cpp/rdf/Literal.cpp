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
bool Literal::is_blank_node() const { return false; }
bool Literal::is_literal() const { return true; }
bool Literal::is_variable() const { return false; }
bool Literal::is_bnode() const { return false; }
bool Literal::is_iri() const { return false; }
Node::RDFNodeType Literal::type() const { return RDFNodeType::Literal; }
Literal::Literal(Node::BackendNodeHandle handle) : Node(handle) {}
std::ostream &operator<<(std::ostream &os, const Literal &literal) {
    os << (std::string) literal;
    return os;
}


}  // namespace rdf4cpp::rdf