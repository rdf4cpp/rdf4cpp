#ifndef RDF4CPP_LITERAL_HPP
#define RDF4CPP_LITERAL_HPP

#include <any>
#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/xsd.hpp>

namespace rdf4cpp::rdf {
class Literal : public Node {
protected:
    Literal(const NodeID &id);
    explicit Literal(Node::BackendNodeHandle handle);

public:
    Literal();
    explicit Literal(const std::string &lexical_form,
                     NodeStorage &node_storage = NodeStorage::primary_instance());
    Literal(const std::string &lexical_form, const IRI &datatype,
            NodeStorage &node_storage = NodeStorage::primary_instance());
    Literal(const std::string &lexical_form, const std::string &lang,
            NodeStorage &node_storage = NodeStorage::primary_instance());

    template<datatypes::DatatypeConcept T>
    explicit Literal(T compatible_type,
                     NodeStorage &node_storage = NodeStorage::primary_instance())
        : Literal((std::string) compatible_type, std::string{T::datatype_iri}, node_storage) {}

    [[nodiscard]] const std::string &lexical_form() const;

    [[nodiscard]] IRI datatype() const;

    [[nodiscard]] const std::string &language_tag() const;


    [[nodiscard]] operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const Literal &literal);

    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    /**
     * Constructs a datatype specific container from Literal.
     * @return
     */
    [[nodiscard]] std::any value() const;
    // TODO: support arithmetics with XSD data types

    template<typename T>
        requires (std::is_constructible_v<T, std::string> || std::is_constructible_v<T, const std::string &> || std::is_constructible_v<T, std::string &&>)
    T value() const {
        return T{this->lexical_form()};
    }

    friend class Node;
};
}  // namespace rdf4cpp::rdf


#endif  //RDF4CPP_LITERAL_HPP
