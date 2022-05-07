#include "Literal.hpp"

#include <rdf4cpp/rdf/IRI.hpp>

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
        // TODO: escape everything that needs to be escaped in N-Tripels/N-Quads
        std::ostringstream out{};
        out << "\"";

        for (auto const &character : lexical) {
            switch (character) {
                case '\n': {
                    out << R"(\n)";
                    break;
                }
                case '\r': {
                    out << R"(\r)";
                    break;
                }
                case '"': {
                    out << R"(\")";
                    break;
                }
                    [[likely]] default : {
                        out << character;
                        break;
                    }
            }
        }
        out << "\"";
        return out.str();
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