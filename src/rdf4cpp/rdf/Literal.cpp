#include "Literal.hpp"

#include <rdf4cpp/rdf/IRI.hpp>
#include <rdf4cpp/rdf/storage/node/LiteralBackend.hpp>

namespace rdf4cpp::rdf {

Literal::Literal(const Node::NodeID &id) : Node(id) {}
Literal::Literal() : Node() {}
Literal::Literal(const std::string &lexical_form, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_string_literal_id(lexical_form)}) {}
Literal::Literal(const std::string &lexical_form, const IRI &datatype, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{
              node_storage.get_typed_literal_id(
                                  lexical_form,
                                  datatype.handle_.id())
                      }) {}
Literal::Literal(const std::string &lexical_form, const std::string &lang, Node::NodeStorage &node_storage)
    : Node(BackendNodeHandle{node_storage.get_lang_literal_id(lexical_form, lang)}) {}


IRI Literal::datatype() const {
    NodeID datatype_id = handle_.literal_backend().datatype_id;
    return IRI(datatype_id);
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
    if (literal.datatype_id.node_id() == NodeID::rdf_langstring_iri.first) {
        return quote_lexical(literal.lexical_form) + "@" + std::string{literal.language_tag};
    } else {
        return quote_lexical(literal.lexical_form) + "^^" + NodeStorage::get_iri_handle(literal.datatype_id).n_string();
    }
}
bool Literal::is_literal() const { return true; }
bool Literal::is_variable() const { return false; }
bool Literal::is_blank_node() const { return false; }
bool Literal::is_iri() const { return false; }
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