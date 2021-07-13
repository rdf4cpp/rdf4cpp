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

    /**
     * Constructs a literal from a compatible type
     * @tparam T a compatible type, i.e. RegisteredDatatype must be specialized for the type
     * @param compatible_value instance for which the literal is created
     * @param node_storage NodeStorage used
     * @return literal instance representing compatible_value
     */
    template<class T>
    inline static Literal make(T compatible_value,
                               NodeStorage &node_storage = NodeStorage::primary_instance()) {
        // TODO: template instantiation should fail if not defined for a type
        return Literal(datatypes::RegisteredDatatype<std::decay_t<T>>::to_string(compatible_value),
                       datatypes::RegisteredDatatype<std::decay_t<T>>::datatype_iri(),
                       node_storage);
    }

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
     * @return std::any wrapped value. might be empty if type is not registered.
     */
    [[nodiscard]] std::any value() const;
    // TODO: support arithmetics with XSD data types

    /**
     * Get the value of an literal. T must be the registered datatype for the datatype iri.
     * @tparam T datatype of the returned instance
     * @return T instance with the value from this
     */
    template<typename T>
    T value() const {
        return datatypes::RegisteredDatatype<std::decay_t<T>>::from_string(this->lexical_form());
    }

    friend class Node;
};
}  // namespace rdf4cpp::rdf


#endif  //RDF4CPP_LITERAL_HPP
