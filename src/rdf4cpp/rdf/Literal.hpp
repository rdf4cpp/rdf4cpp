#ifndef RDF4CPP_LITERAL_HPP
#define RDF4CPP_LITERAL_HPP

#include <any>
#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/xsd.hpp>

namespace rdf4cpp::rdf {
class Literal : public Node {
private:
    /**
     * @brief the implementation for all numeric, binary operations
     *
     * @tparam OpSelect a function NumericOps -> binop_fptr_t
     * @param op_select is used to select the specific operation to be carried out
     * @param other rhs of the operation
     * @param node_storage the node storage that the resulting value will be put in
     * @return the literal resulting from the selected binop or Literal{} if the types are not
     *      convertible to a common type or the common type is not numeric
     */
    template<typename OpSelect>
    Literal numeric_binop_impl(OpSelect op_select, Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;

    /**
     * @brief the implementation for all numeric, unary operations
     *
     * @tparam OpSelect a function NumericOps -> unop_fptr_t
     * @param op_select is used to select the specific operation to be carried out
     * @param node_storage the node storage that the resulting value will be put in
     * @return the literal resulting from the selected unop or Literal{} if this is not numeric
     */
    template<typename OpSelect>
    Literal numeric_unop_impl(OpSelect op_select, NodeStorage &node_storage = NodeStorage::default_instance()) const;

    enum struct TriStateBool : size_t {
        Err = 0,
        False = 1,
        True = 2,
    };

    /**
     * @return the effective-boolean-value of this literal as a TriStateBool
     */
    TriStateBool get_ebv_impl() const;

    /**
     * @brief the implementation for all logical, binary operations
     *
     * @param logic_table the logic table for the binary operation. It is accessed via
     *      logic_table[static_cast<size_t>(this->get_ebv_impl())][static_cast<size_t>(other.get_ebv_impl())].
     *      For an example logic table see operator&& or operator||.
     * @param other the lhs of the operation
     * @param node_storage the node storage that the resulting value will be put in
     * @return the literal resulting by converting both literals to their ebv and applying the provided binop or Literal{}
     *      if at least one of the types is not convertible to bool
     */
    Literal logical_binop_impl(std::array<std::array<TriStateBool, 3>, 3> const &logic_table, Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;

protected:
    explicit Literal(Node::NodeBackendHandle handle);

public:
    Literal() noexcept;
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
     * Constructs a literal from a compatible type. In this version of the function the datatype is specified at compile time.
     * No runtime lookup of the type information is required.
     * If type information is available at compile time, you should use this version of the function.
     * @tparam T a compatible type, i.e. RegisteredDatatype must be specialized for the type
     * @tparam dtype_iri IRI string of the RDF datatype
     * @param compatible_value instance for which the literal is created
     * @param node_storage NodeStorage used
     * @return literal instance representing compatible_value
     */
    template<datatypes::LiteralDatatype LiteralDatatype_t>
    inline static Literal make(typename LiteralDatatype_t::cpp_type compatible_value,
                               NodeStorage &node_storage = NodeStorage::default_instance()) {
        return Literal(LiteralDatatype_t::to_string(compatible_value),
                       IRI(LiteralDatatype_t::identifier, node_storage),
                       node_storage);
    }

    /**
     * Constructs a literal from a compatible type. In this version of the function the datatype is specified at runtime.
     * Due to the lookup of the converter functions, this function is slightly slower than its templated version.
     * @param lexical_form
     * @param datatype
     * @param node_storage
     * @return
     */
    inline static Literal make(std::string_view lexical_form, const IRI &datatype,
                               NodeStorage &node_storage = NodeStorage::default_instance());

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
    [[nodiscard]] bool is_numeric() const;

    bool operator==(const Literal &other) const;

    std::partial_ordering operator<=>(const Literal &other) const;

    Literal add(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator+(Literal const &other) const;

    Literal sub(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator-(Literal const &other) const;

    Literal mul(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator*(Literal const &other) const;

    Literal div(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator/(Literal const &other) const;

    Literal pos(NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator+() const;

    Literal neg(NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator-() const;

    /**
     * Converts this literal to it's effective boolean value
     * @return Literal containing the ebv
     */
    Literal effective_boolean_value(NodeStorage &node_storage = NodeStorage::default_instance()) const;

    Literal logical_and(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator&&(Literal const &other) const;

    Literal logical_or(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator||(Literal const &other) const;

    Literal logical_not(NodeStorage &node_storage = NodeStorage::default_instance()) const;
    Literal operator!() const;

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
    template<datatypes::LiteralDatatype LiteralDatatype_t>
    typename LiteralDatatype_t::cpp_type value() const {
        return LiteralDatatype_t::from_string(this->lexical_form());
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
