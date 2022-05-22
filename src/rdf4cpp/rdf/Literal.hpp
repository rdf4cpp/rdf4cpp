#ifndef RDF4CPP_LITERAL_HPP
#define RDF4CPP_LITERAL_HPP

#include <any>
#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/xsd.hpp>

namespace rdf4cpp::rdf {
class Literal : public Node {
protected:
    explicit Literal(Node::NodeBackendHandle handle);

public:
    Literal();
    /**
     * Constructs a Literal from a lexical form. Datatype is `xsd:string`.
     * @param lexical_form the lexical form
     * @param node_storage optional custom node_storage used to store the literal
     */
    explicit Literal(std::string_view lexical_form,
                     NodeStorage &node_storage = NodeStorage::default_instance());
    /**
     * Constructs a Literal from a lexical form and a datatype.
     * @param lexical_form the lexical form
     * @param datatype the datatype
     * @param node_storage optional custom node_storage used to store the literal
     */
    Literal(std::string_view lexical_form, const IRI &datatype,
            NodeStorage &node_storage = NodeStorage::default_instance());
    /**
     * Constructs a Literal from a lexical form and a language tag. The datatype is `rdf:langString`.
     * @param lexical_form the lexical form
     * @param lang the language tag
     * @param node_storage optional custom node_storage used to store the literal
     */
    Literal(std::string_view lexical_form, std::string_view lang,
            NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs a literal from a compatible type
     * @tparam T a compatible type, i.e. RegisteredDatatype must be specialized for the type
     * @tparam dtype_iri IRI string of the RDF datatype
     * @param compatible_value instance for which the literal is created
     * @param node_storage NodeStorage used
     * @return literal instance representing compatible_value
     */
    template<typename T, datatypes::ConstexprString dtype_iri>
    inline static Literal make(T compatible_value,
                               NodeStorage &node_storage = NodeStorage::default_instance()) {
        return Literal(datatypes::RegisteredDatatype<std::decay_t<T>, dtype_iri>::to_string(compatible_value),
                       IRI(datatypes::RegisteredDatatype<std::decay_t<T>, dtype_iri>::datatype_iri(), node_storage),
                       node_storage);
    }

    /**
     * Returns the lexical from of this. The lexical form is the part of the identifier that encodes the value. So datatype and language_tag are not part of the lexical form.
     * E.g. For `"abc"^^xsd::string` the lexical form is `abc`
     * @return lexical form
     */
    [[nodiscard]] std::string_view lexical_form() const;

    /**
     * Returns the datatype IRI of this.
     * @return datatype IRI
     */
    [[nodiscard]] IRI datatype() const;

    /**
     * Returns the language tag of this Literal. If the string is empty this has no lanugage tag.
     * @return language tag
     */
    [[nodiscard]] std::string_view language_tag() const;

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const Literal &literal);

    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_iri() const;

    bool operator==(const Literal &other) const;

    std::partial_ordering operator<=>(const Literal &other) const;

    /**
     * Constructs a datatype specific container from Literal.
     * @return std::any wrapped value. might be empty if type is not registered.
     */
    [[nodiscard]] std::any value() const;

    /**
     * Get the value of an literal. T must be the registered datatype for the datatype iri.
     * @tparam T datatype of the returned instance
     * @return T instance with the value from this
     */
    template<typename T, datatypes::ConstexprString dtype_iri>
    T value() const {
        // TODO: later, add support have a default template parameter for the second template parameter based on the first
        return datatypes::RegisteredDatatype<std::decay_t<T>, dtype_iri>::from_string(this->lexical_form());
    }

    friend class Node;
};
}  // namespace rdf4cpp::rdf

template<>
struct std::hash<rdf4cpp::rdf::Literal> {
    inline size_t operator()(rdf4cpp::rdf::Literal const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_LITERAL_HPP
