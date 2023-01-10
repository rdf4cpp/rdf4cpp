#ifndef RDF4CPP_LITERAL_HPP
#define RDF4CPP_LITERAL_HPP

#include <any>
#include <ostream>
#include <type_traits>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/rdf.hpp>
#include <rdf4cpp/rdf/datatypes/xsd.hpp>
#include <rdf4cpp/rdf/datatypes/owl.hpp>
#include <rdf4cpp/rdf/util/TriBool.hpp>
#include <rdf4cpp/rdf/util/CowString.hpp>

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
        requires std::is_nothrow_invocable_r_v<datatypes::registry::DatatypeRegistry::binop_fptr_t, OpSelect, datatypes::registry::DatatypeRegistry::NumericOpsImpl const &>
    [[nodiscard]] Literal numeric_binop_impl(OpSelect op_select, Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @brief the implementation for all numeric, unary operations
     *
     * @tparam OpSelect a function NumericOps -> unop_fptr_t
     * @param op_select is used to select the specific operation to be carried out
     * @param node_storage the node storage that the resulting value will be put in
     * @return the literal resulting from the selected unop or Literal{} if this is not numeric
     */
    template<typename OpSelect>
        requires std::is_nothrow_invocable_r_v<datatypes::registry::DatatypeRegistry::unop_fptr_t, OpSelect, datatypes::registry::DatatypeRegistry::NumericOpsImpl const &>
    [[nodiscard]] Literal numeric_unop_impl(OpSelect op_select, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @brief the implementation of the value comparison function
     *
     * @param other the literal to compare to
     * @param out_alternative_ordering optional out parameter to receive an alternative ordering (for ordering extensions).
     *      Note: If present it is expected to be defaulted to std::strong_ordering::equivalent.
     *      It is populated based on the following rules:
     *          - (null, non-null) => less
     *          - (non-null, null) => greater
     *          - (non-null, non-null) and equal type => lexical form ordering
     *          - (non-null, non-null) and different type => type ordering
     *
     * @return the ordering of the values of this and other; if there is a value ordering
     */
    std::partial_ordering compare_impl(Literal const &other, std::strong_ordering *out_alternative_ordering = nullptr) const noexcept;

    /**
     * get the DatatypeIDView for the datatype of *this,
     * it will always contain the appropriate id type
     * and can be used to index the registry
     */
    [[nodiscard]] datatypes::registry::DatatypeIDView datatype_id() const noexcept;

    /**
     * @return if the datatype of this is fixed
     */
    [[nodiscard]] bool is_fixed() const noexcept;

    /**
     * @return if the datatype of this is simultaneously fixed but not numeric
     */
    [[nodiscard]] bool is_fixed_not_numeric() const noexcept;


    /**
     * Creates a simple Literal directly without any safety checks
     */
    [[nodiscard]] static Literal make_simple_unchecked(std::string_view lexical_form, NodeStorage &node_storage) noexcept;

    /**
     * Creates a non-inlined typed Literal without doing any safety checks or canonicalization.
     */
    [[nodiscard]] static Literal make_noninlined_typed_unchecked(std::string_view lexical_form, IRI const &datatype, NodeStorage &node_storage) noexcept;

    /**
     * Creates a language-tagged Literal directly without any safety checks
     */
    [[nodiscard]] static Literal make_lang_tagged_unchecked(std::string_view lexical_form, std::string_view lang, NodeStorage &node_storage) noexcept;

    /**
     * Creates an inlined Literal without any safety checks
     *
     * @param inlined_value a valid inlined value for the given datatype (identified via a fixed_id) packed into the lower LiteralID::width bits of the integer
     * @note inlined_values for a datatype can be obtained via Datatype::try_into_inlined(value) if the datatype is inlineable (see registry::capabilities::Inlineable)
     */
    [[nodiscard]] static Literal make_inlined_typed_unchecked(uint64_t inlined_value, storage::node::identifier::LiteralType fixed_id, NodeStorage &node_storage) noexcept;

    /**
     * Creates an inlined or non-inlined typed Literal without any safety checks
     */
    [[nodiscard]] static Literal make_typed_unchecked(std::any const &value, datatypes::registry::DatatypeIDView datatype, datatypes::registry::DatatypeRegistry::DatatypeEntry const &entry, NodeStorage &node_storage) noexcept;
protected:
    explicit Literal(Node::NodeBackendHandle handle) noexcept;

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
    static Literal make(typename LiteralDatatype_t::cpp_type compatible_value,
                        NodeStorage &node_storage = NodeStorage::default_instance()) noexcept {

        if constexpr (std::is_same_v<LiteralDatatype_t, datatypes::rdf::LangString>) {
            return Literal::make_lang_tagged_unchecked(compatible_value.lexical_form,
                                                       compatible_value.language_tag,
                                                       node_storage);
        }

        if constexpr (datatypes::IsInlineable<LiteralDatatype_t>) {
            if (auto const maybe_inlined = LiteralDatatype_t::try_into_inlined(compatible_value); maybe_inlined.has_value()) {
                return Literal::make_inlined_typed_unchecked(*maybe_inlined, LiteralDatatype_t::datatype_id.get_fixed(), node_storage);
            }
        }

        return Literal::make_noninlined_typed_unchecked(LiteralDatatype_t::to_string(compatible_value),
                                                        IRI{LiteralDatatype_t::datatype_id, node_storage},
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
    static Literal make(std::string_view lexical_form, const IRI &datatype,
                        NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Tries to cast this literal to a literal of the given type IRI.
     *
     * @param target the IRI of the cast target
     * @param node_storage where to store the literal resulting from the cast
     * @return the literal with the same value as a different type if the cast was successful or the null literal
     */
    [[nodiscard]] Literal cast(IRI const &target, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * Identical to Literal::cast except with compile time specified target type.
     */
    template<datatypes::LiteralDatatype LiteralDatatype_t>
    [[nodiscard]] Literal cast(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept {
        return this->cast(IRI{LiteralDatatype_t::datatype_id, node_storage}, node_storage);
    }

    /**
     * Returns the lexical from of this. The lexical form is the part of the identifier that encodes the value. So datatype and language_tag are not part of the lexical form.
     * E.g. For `"abc"^^xsd::string` the lexical form is `abc`
     * @return lexical form
     */
    [[nodiscard]] util::CowString lexical_form() const noexcept;

    /**
     * Converts this into it's lexical form as xsd:string. See Literal::lexical_form for more details.
     *
     * @param node_storage where to put the resulting literal
     * @return lexical form of this as xsd:string
     */
    [[nodiscard]] Literal as_lexical_form(NodeStorage &node_storage = NodeStorage::default_instance()) const;

    /**
     * Returns the datatype IRI of this.
     * @return datatype IRI
     */
    [[nodiscard]] IRI datatype() const noexcept;

    /**
     * Returns the language tag of this Literal. If the string is empty this has no lanugage tag.
     * @return language tag
     */
    [[nodiscard]] std::string_view language_tag() const noexcept;

    [[nodiscard]] explicit operator std::string() const noexcept;

    friend std::ostream &operator<<(std::ostream &os, const Literal &literal);

    /**
     * Constructs a datatype specific container from Literal.
     * @return std::any wrapped value. might be empty if type is not registered.
     */
    [[nodiscard]] std::any value() const;

    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_iri() const noexcept;
    [[nodiscard]] bool is_numeric() const noexcept;

    /**
     * The default (value-only) comparison function
     * without SPARQL operator extensions.
     *
     * @return the value ordering of this and other
     */
    [[nodiscard]] std::partial_ordering compare(Literal const &other) const noexcept;

    /**
     * A convenient (and equivalent) alternative to compare.
     */
    std::partial_ordering operator<=>(Literal const &other) const noexcept;

    /**
     * The comparison function with SPARQL operator extensions.
     *
     * @return similar to `compare` but:
     *      - values of an incomparable type are all considered equivalent
     *      - a null literal is the smallest possible value of all types
     *      - the type ordering replaces the value ordering in the following cases
     *          - the values are equal
     *          - at least one of the value's types is not comparable
     *          - there is no viable conversion to a common type to check for equality
     */
    [[nodiscard]] std::weak_ordering compare_with_extensions(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool eq(Literal const &other) const noexcept;
    util::TriBool operator==(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool ne(Literal const &other) const noexcept;
    util::TriBool operator!=(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool lt(Literal const &other) const noexcept;
    util::TriBool operator<(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool le(Literal const &other) const noexcept;
    util::TriBool operator<=(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool gt(Literal const &other) const noexcept;
    util::TriBool operator>(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool ge(Literal const &other) const noexcept;
    util::TriBool operator>=(Literal const &other) const noexcept;

    [[nodiscard]] bool eq_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] bool ne_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] bool lt_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] bool le_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] bool gt_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] bool ge_with_extensions(Literal const &other) const noexcept;

    [[nodiscard]] Literal add(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator+(Literal const &other) const noexcept;

    [[nodiscard]] Literal sub(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator-(Literal const &other) const noexcept;

    [[nodiscard]] Literal mul(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator*(Literal const &other) const noexcept;

    [[nodiscard]] Literal div(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator/(Literal const &other) const noexcept;

    [[nodiscard]] Literal pos(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator+() const noexcept;

    [[nodiscard]] Literal neg(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator-() const noexcept;

    /**
     * @return the effective boolean value of this
     */
    [[nodiscard]] util::TriBool ebv() const noexcept;

    /**
     * Converts this literal to it's effective boolean value
     * @return Literal containing the ebv
     */
    [[nodiscard]] Literal ebv_as_literal(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    [[nodiscard]] Literal logical_and(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator&&(Literal const &other) const noexcept;

    [[nodiscard]] Literal logical_or(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator||(Literal const &other) const noexcept;

    [[nodiscard]] Literal logical_not(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator!() const noexcept;

    /**
     * Get the value of an literal. T must be the registered datatype for the datatype iri.
     * @tparam T datatype of the returned instance
     * @return T instance with the value from this
     */
    template<datatypes::LiteralDatatype LiteralDatatype_t>
    typename LiteralDatatype_t::cpp_type value() const {
        if constexpr (datatypes::IsInlineable<LiteralDatatype_t>) {
            if (this->is_inlined()) {
                auto const inlined_value = this->handle_.node_id().literal_id().value;
                return LiteralDatatype_t::from_inlined(inlined_value);
            }
        }

        if constexpr (std::is_same_v<LiteralDatatype_t, datatypes::rdf::LangString>) {
            auto const &lit = this->handle_.literal_backend();

            return datatypes::registry::LangStringRepr{
                    .lexical_form = std::string{lit.lexical_form},
                    .language_tag = std::string{lit.language_tag}};
        }

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
