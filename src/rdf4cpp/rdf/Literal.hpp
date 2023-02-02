#ifndef RDF4CPP_LITERAL_HPP
#define RDF4CPP_LITERAL_HPP

#include <any>
#include <ostream>
#include <type_traits>
#include <optional>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/rdf.hpp>
#include <rdf4cpp/rdf/datatypes/xsd.hpp>
#include <rdf4cpp/rdf/datatypes/owl.hpp>
#include <rdf4cpp/rdf/regex/Regex.hpp>
#include <rdf4cpp/rdf/util/TriBool.hpp>
#include <rdf4cpp/rdf/util/CowString.hpp>

namespace rdf4cpp::rdf {
class Literal : public Node {
private:
    /**
     * the implementation for all numeric, binary operations
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
     * the implementation for all numeric, unary operations
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
     * @return if this datatype is either xsd:string or rdf:langString
     */
    [[nodiscard]] bool is_string_like() const noexcept;

    /**
     * Creates a simple Literal directly without any safety checks
     */
    [[nodiscard]] static Literal make_simple_unchecked(std::string_view lexical_form, NodeStorage &node_storage) noexcept;

    /**
     * Creates a non-inlined typed Literal without doing any safety checks or canonicalization.
     */
    [[nodiscard]] static Literal make_noninlined_typed_unchecked(std::string_view lexical_form, IRI const &datatype, NodeStorage &node_storage) noexcept;

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

    /**
     * Creates a language-tagged Literal directly without any safety checks
     */
    [[nodiscard]] static Literal make_lang_tagged_unchecked(std::string_view lexical_form, std::string_view lang, NodeStorage &node_storage) noexcept;

    /**
     * Creates a string like type with contents of str.
     * Will either create
     *      - a xsd:string if lang_tag_src is xsd:string
     *      - a rdf:langString with language tag equal to that of lang_tag_src if lang_tag_src is an rdf:langString
     *
     * @param str lexical form of the to be created string
     * @param lang_tag_src source for the language tag of the newly created string
     * @return a string like type with str as lexical form and the language tag (if any) of lang_tag_src
     */
    [[nodiscard]] static Literal make_string_like_copy_lang_tag(std::string_view str, Literal const &lang_tag_src, NodeStorage &node_storage) noexcept;

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

        return Literal::make_noninlined_typed_unchecked(LiteralDatatype_t::to_canonical_string(compatible_value),
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
    static Literal make(std::string_view lexical_form, IRI const &datatype,
                        NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs a literal from a tri-bool with the following mappings
     *      - TriBool::True => "true"^^xsd:boolean
     *      - TriBool::False => "false"^^xsd:boolean
     *      - TriBool::Err => the null literal
     *
     * @return the literal form of the given boolean
     */
    static Literal make_boolean(util::TriBool b, NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Generates a random double in the range [0.0, 1.0).
     * The values are generated in a thread-safe manner using a lazily initialized thread_local random generator.
     *
     * @see https://www.w3.org/TR/sparql11-query/#idp2130040
     */
    [[nodiscard]] static Literal generate_random_double(NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Checks if the datatype of this matches the provided LiteralDatatype
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @tparam T the datatype to compare against
     * @return true iff this datatype is T
     */
    template<datatypes::LiteralDatatype T>
    [[nodiscard]] bool datatype_matches() const noexcept {
        return this->datatype_id() == T::datatype_id;
    }

    /**
     * Checks if the datatype of this matches the given IRI
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @param datatype the datatype to compare against
     * @return true iff this datatype is datatype
     */
    [[nodiscard]] bool datatype_matches(IRI const &datatype) const noexcept;

    /**
     * Checks if the datatype of this matches the datatype of other
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @param other other literal to check against
     * @return true iff this' datatype matches other's datatype
     */
    [[nodiscard]] bool datatype_matches(Literal const &other) const noexcept;

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
     * @return lexical form of this as xsd:string if this is not the null literal, otherwise returns the null literal
     */
    [[nodiscard]] Literal as_lexical_form(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * Returns the simplified/more user friendly string version of this. This is for example used when casting numerics to string.
     * @return user friendly string representation
     */
    [[nodiscard]] util::CowString simplified_lexical_form() const noexcept;

    /**
     * Converts this into it's simplified/more user friendly string representation as xsd:string. See Literal::to_simplified_string for more details.
     * @return user friendly string representation of this as xsd:string if this is not the null literal, otherwise returns the null literal
     */
    [[nodiscard]] Literal as_simplified_lexical_form(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

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
    [[nodiscard]] std::any value() const noexcept;

    /**
     * Get the value of an literal. T must be the registered datatype for the datatype iri.
     * @tparam T datatype of the returned instance
     * @return T instance with the value from this
     */
    template<datatypes::LiteralDatatype LiteralDatatype_t>
    typename LiteralDatatype_t::cpp_type value() const {
        if (this->datatype_id() != LiteralDatatype_t::datatype_id) [[unlikely]] {
            throw std::runtime_error{"Literal::value error: incompatible type"};
        }

        if constexpr (datatypes::IsInlineable<LiteralDatatype_t>) {
            if (this->is_inlined()) {
                auto const inlined_value = this->handle_.node_id().literal_id().value;
                return LiteralDatatype_t::from_inlined(inlined_value);
            }
        }

        if constexpr (std::is_same_v<LiteralDatatype_t, datatypes::rdf::LangString>) {
            auto const &lit = this->handle_.literal_backend();

            return datatypes::registry::LangStringRepr{
                    .lexical_form = lit.lexical_form,
                    .language_tag = lit.language_tag};
        }

        return LiteralDatatype_t::from_string(this->lexical_form());
    }

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
     * @see https://www.w3.org/TR/xpath-functions/#func-abs
     * @return absolute value of this or the null literal if this is not numeric
     */
    [[nodiscard]] Literal abs(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-round
     * @return the rounded value of this or the null literal if this is not numeric
     */
    [[nodiscard]] Literal round(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-floor
     * @return the floored value of this or the null literal if this is not numeric
     */
    [[nodiscard]] Literal floor(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-ceiling
     * @return the ceiled value of this or the null literal if this is not numeric
     */
    [[nodiscard]] Literal ceil(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-string-length
     * @return the length this' lexical form if it is string-like otherwise nullopt
     */
    [[nodiscard]] std::optional<size_t> strlen() const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-string-length
     * @return the length this' lexical form (as xsd:integer) if it is string-like otherwise the null literal
     */
    [[nodiscard]] Literal as_strlen(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/sparql11-query/#func-langMatches
     * @param lang_range a basic language range
     * @return whether the language tag of this matches the given lang range if this is string-like, otherwise Err
     */
    [[nodiscard]] util::TriBool lang_matches(std::string_view lang_range) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-string-length
     * @param lang_range a basic language range as xsd:string
     * @return whether the language tag of this matches the given lang range or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal as_lang_matches(std::string_view lang_range, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-string-length
     * @param lang_range a basic language range as xsd:string
     * @return whether the language tag of this matches the given lang range or the null literal if
     *      - this is not string-like
     *      - lang_range is not xsd:string
     */
    [[nodiscard]] Literal as_lang_matches(Literal const &lang_range, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-matches
     *
     * @param pattern regex to match against
     * @return whether this' lexical form matches the regex or Err if this is not string-like
     */
    [[nodiscard]] util::TriBool regex_matches(regex::Regex const &pattern) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-matches
     *
     * @param pattern xsd:string containing a regex to match against
     * @param flags regex flags to use for matching (https://www.w3.org/TR/xpath-functions/#flags)
     * @return whether this' lexical form matches the regex or the null literal if
     *      - this is not string-like
     *
     * @note currently only flags `m` and `i` are supported, the other valid flags from the XPATH standard will be ignored
     * @todo implement other flags
     */
    [[nodiscard]] Literal as_regex_matches(regex::Regex const &pattern, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-matches
     *
     * @param pattern xsd:string containing a regex to match against
     * @param flags regex flags to use for matching (https://www.w3.org/TR/xpath-functions/#flags)
     * @return whether this' lexical form matches the regex or the null literal if
     *      - this is not string-like
     *      - regex is not string-like
     *      - flags is not string-like or not parsable as flags
     *
     * @note currently only flags `m` and `i` are supported, the other valid flags from the XPATH standard will be ignored
     * @todo implement other flags
     */
    [[nodiscard]] Literal as_regex_matches(Literal const &pattern, Literal const &flags = Literal{""}, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-replace
     *
     * @param pattern regex (as string) to match against
     * @param replacement string to replace the matched pattern
     * @return the new string with the matches substring replaced by replacement
     */
    [[nodiscard]] Literal regex_replace(regex::RegexReplacer const &replacer, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-replace
     *
     * @param pattern regex (as string) to match against
     * @param replacement string to replace the matched pattern
     * @param flags regex flags to use for matching (https://www.w3.org/TR/xpath-functions/#flags)
     * @return the new string with the matches substring replaced by replacement
     *
     * @note currently only flags `m` and `i` are supported, the other valid flags from the XPATH standard will be ignored
     * @todo implement other flags
     */
    [[nodiscard]] Literal regex_replace(Literal const &pattern, Literal const &replacement,
                                        Literal const &flags = Literal{""},
                                        NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-contains
     *
     * @param needle substring to search for in this
     * @return whether this' lexical form contains the given string or Err if this is not string-like
     */
    [[nodiscard]] util::TriBool contains(std::string_view needle) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-contains
     *
     * @param needle substring to search for in this
     * @return whether this' lexical form contains the given string (as xsd:boolean) or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal as_contains(std::string_view needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-contains
     *
     * @param needle substring to search for in this
     * @return whether this' lexical form contains the given string (as xsd:boolean) or the null literal if
     *      - this is not string-like
     *      - needle is not string-like
     */
    [[nodiscard]] Literal as_contains(Literal const &needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-substring-before
     *
     * @param needle substring to search for in this
     * @return substring of this' up to the position of the needle
     *      or null literal if
     *          - this is not string-like
     *      or empty string if
     *          - needle is the empty string
     *          - needle could not be found in this
     */
    [[nodiscard]] Literal substr_before(std::string_view needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-substring-before
     *
     * @param needle string-like value to search for in this
     * @return substring of this' up to the position of the needle
     *      or null literal if
     *          - this is not string-like
     *          - needle is not string-like
     *      or empty string if
     *          - needle is the empty string
     *          - needle could not be found in this
     */
    [[nodiscard]] Literal substr_before(Literal const &needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-substring-after
     *
     * @param needle substring to search for in this
     * @return substring of this' after the end of the needle
     *      or null literal if
     *          - this is not string-like
     *      or empty string if
     *          - needle is the empty string
     *          - needle could not be found in this
     */
    [[nodiscard]] Literal substr_after(std::string_view needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-substring-after
     *
     * @param needle substring to search for in this
     * @return substring of this' after the end of the needle
     *      or null literal if
     *          - this is not string-like
     *          - needle is not string-like
     *      or empty string if
     *          - needle is the empty string
     *          - needle could not be found in this
     */
    [[nodiscard]] Literal substr_after(Literal const &needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-starts-with
     *
     * @param needle substring to check
     * @return whether this' lexical form starts with needle or Err if
     *      - this is not string-like
     */
    [[nodiscard]] util::TriBool str_starts_with(std::string_view needle) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-starts-with
     *
     * @param needle substring to check
     * @return whether this' lexical form starts with needle (as xsd:boolean) or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal as_str_starts_with(std::string_view needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-starts-with
     *
     * @param needle substring to check
     * @return whether this' lexical form starts with needle (as xsd:boolean) or the null literal if
     *      - this is not string-like
     *      - needle is not string-like
     *      - the language tags of this and needle do not match
     */
    [[nodiscard]] Literal as_str_starts_with(Literal const &needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-ends-with
     *
     * @param needle substring to check
     * @return whether this' lexical form ends with needle or Err if
     *      - this is not string-like
     */
    [[nodiscard]] util::TriBool str_ends_with(std::string_view needle) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-ends-with
     *
     * @param needle substring to check
     * @return whether this' lexical form ends with needle (as xsd:boolean) or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal as_str_ends_with(std::string_view needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-ends-with
     *
     * @param needle substring to check
     * @return whether this' lexical form ends with needle (as xsd:boolean) or the null literal if
     *      - this is not string-like
     *      - needle is not string-like
     *      - the language tags of this and needle do not match
     */
    [[nodiscard]] Literal as_str_ends_with(Literal const &needle, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-upper-case
     *
     * @return the upper case version if this' if this is string-like or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal uppercase(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-lower-case
     *
     * @return the lower case version if this' if this is string-like or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal lowercase(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-concat
     *
     * @param other other literal to append to this
     * @return a string-like type that is the concatenation of the lexical forms of this and other
     */
    [[nodiscard]] Literal concat(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * akin to std::string_view::substr
     * @see https://en.cppreference.com/w/cpp/string/basic_string_view/substr
     * @warning 0-based indexing
     *
     * @param start position of the first character
     * @param len requested length of the substring
     * @return the characters in this whose position P satisfy: P >= start && P < start + len or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal substr(size_t start,
                                 size_t len = std::string_view::npos,
                                 NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-substring
     * @warning 1-based indexing
     *
     * @param start position of the first character
     * @param len requested length of the substring
     * @return the characters in this whose position P satisfy: P >= round(start) && P < round(start) + round(len) or the null literal if
     *      - this is not string-like
     *      - start is not xsd:double or derived from it
     *      - len is not xsd:double or derived from it
     */
    [[nodiscard]] Literal substr(Literal const &start,
                                 Literal const &len = Literal::make<datatypes::xsd::Double>(std::numeric_limits<datatypes::xsd::Double::cpp_type>::infinity()),
                                 NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

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

    friend class Node;
};

inline namespace shorthands {

Literal operator""_xsd_string(char const *str, size_t len);

Literal operator""_xsd_double(long double d);
Literal operator""_xsd_float(long double d);

Literal operator""_xsd_decimal(char const *str, size_t len);

Literal operator""_xsd_integer(unsigned long long int i);

Literal operator""_xsd_byte(unsigned long long int i);
Literal operator""_xsd_ubyte(unsigned long long int i);

Literal operator""_xsd_short(unsigned long long int i);
Literal operator""_xsd_ushort(unsigned long long int i);

Literal operator""_xsd_int(unsigned long long int i);
Literal operator""_xsd_uint(unsigned long long int i);

Literal operator""_xsd_long(unsigned long long int i);
Literal operator""_xsd_ulong(unsigned long long int i);

}  // namespace shorthands
}  // namespace rdf4cpp::rdf

template<>
struct std::hash<rdf4cpp::rdf::Literal> {
    inline size_t operator()(rdf4cpp::rdf::Literal const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_LITERAL_HPP
