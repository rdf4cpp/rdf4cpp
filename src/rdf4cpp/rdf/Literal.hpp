#ifndef RDF4CPP_LITERAL_HPP
#define RDF4CPP_LITERAL_HPP


#include <any>
#include <optional>
#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/LiteralDatatype.hpp>
#include <rdf4cpp/rdf/datatypes/owl.hpp>
#include <rdf4cpp/rdf/datatypes/rdf.hpp>
#include <rdf4cpp/rdf/datatypes/xsd.hpp>
#include <rdf4cpp/rdf/regex/Regex.hpp>
#include <rdf4cpp/rdf/util/CowString.hpp>
#include <rdf4cpp/rdf/util/Overloaded.hpp>
#include <rdf4cpp/rdf/util/TriBool.hpp>
#include <type_traits>

namespace rdf4cpp::rdf {

/**
 * An RDF Literal.
 *
 * Functions behave based on the following rules:
 * - public transformation functions (i.e. Literal -> Literal functions, that are usually called as_*) check for null and return back a null literal
 * - public is_* functions check for null and return an appropriate value for null-literals
 * - other public functions (i.e. Literal -> non-Literal) usually do not check for null (e.g. Literal::lexical_form),
 *      but if the null-Literal has a meaningful value for that function it will behave correctly (e.g. for null-Literal <=> non-null-Literal)
 */
class Literal : public Node {
private:
    [[nodiscard]] static bool lexical_form_needs_escape(std::string_view lexical_form) noexcept;

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
    [[nodiscard]] static Literal make_simple_unchecked(std::string_view lexical_form, bool needs_escape, NodeStorage &node_storage) noexcept;

    /**
     * Creates a non-inlined typed Literal without doing any safety checks or canonicalization.
     */
    [[nodiscard]] static Literal make_noninlined_typed_unchecked(std::string_view lexical_form, bool needs_escape, IRI const &datatype, NodeStorage &node_storage) noexcept;

    [[nodiscard]] static Literal make_noninlined_special_unchecked(std::any &&value, storage::node::identifier::LiteralType fixed_id, NodeStorage &node_storage) noexcept;

    /**
     * Creates an inlined Literal without any safety checks
     *
     * @param inlined_value a valid inlined value for the given datatype (identified via a fixed_id) packed into the lower LiteralID::width bits of the integer
     * @note inlined_values for a datatype can be obtained via Datatype::try_into_inlined(value) if the datatype is inlineable (see registry::capabilities::Inlineable)
     */
    [[nodiscard]] static Literal make_inlined_typed_unchecked(storage::node::identifier::LiteralID inlined_value, storage::node::identifier::LiteralType fixed_id, NodeStorage const &node_storage) noexcept;

    /**
     * Creates an inlined or non-inlined typed Literal without any safety checks
     */
    [[nodiscard]] static Literal make_typed_unchecked(std::any &&value, datatypes::registry::DatatypeIDView datatype, datatypes::registry::DatatypeRegistry::DatatypeEntry const &entry, NodeStorage &node_storage) noexcept;

    /**
     * Creates a language-tagged Literal directly without any safety checks
     */
    [[nodiscard]] static Literal make_lang_tagged_unchecked(std::string_view lexical_form, bool needs_escape, std::string_view lang, NodeStorage &node_storage) noexcept;

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
     // TODO needs escape flag
    [[nodiscard]] static Literal make_string_like_copy_lang_tag(std::string_view str, Literal const &lang_tag_src, NodeStorage &node_storage) noexcept;

    /**
     * Creates a normal accessible Literal from a lang tagged string.
     * Do not leak this, this would make lang tag inlining useless.
     * note: no safety checks here, make sure this is actually a inlined language tagged string before calling!
     * @return de inlined lang tagged literal
     */
    [[nodiscard]] Literal lang_tagged_get_de_inlined() const noexcept;

    /**
     * Checks if the dynamic datatype of this is equal to datatype
     * @warning must only be called if this has dynamic datatype
     *
     * @param datatype datatype to check against
     * @return true iff this dynamic datatype matches datatype
     *
     * @note this function only exists because IRI is an incomplete type in this header
     *      which prevents the template version of datatype_eq from using IRIs
     */
    [[nodiscard]] bool dynamic_datatype_eq_impl(std::string_view datatype) const noexcept;
protected:
    explicit Literal(Node::NodeBackendHandle handle) noexcept;

public:
    /**
     * Constructs the null-literal
     */
    Literal() noexcept;

    /**
     * Constructs the null-literal
     */
    [[nodiscard]] static Literal make_null() noexcept;

    /**
     * Constructs a simple Literal from a lexical form. Datatype is `xsd:string`.
     * @param lexical_form the lexical form
     * @param node_storage optional custom node_storage used to store the literal
     * @throws std::runtime_error if lexical_form contains invalid unicode
     */
    [[nodiscard]] static Literal make_simple(std::string_view lexical_form, NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs a simple Literal from a lexical form. Datatype is `xsd:string`.
     * normalizes lexical_form to UTF-8 NFC.
     * @param lexical_form the lexical form
     * @param node_storage optional custom node_storage used to store the literal
     */
    [[nodiscard]] static Literal make_simple_normalize(std::string_view lexical_form, NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs a Literal from a lexical form and a language tag. The datatype is `rdf:langString`.
     * @param lexical_form the lexical form
     * @param lang the language tag
     * @param node_storage optional custom node_storage used to store the literal
     * @throws std::runtime_error if lexical_form contains invalid unicode
     */
    [[nodiscard]] static Literal make_lang_tagged(std::string_view lexical_form, std::string_view lang_tag,
                                                  NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs a Literal from a lexical form and a language tag. The datatype is `rdf:langString`.
     * normalizes lexical_form to UTF-8 NFC.
     * @param lexical_form the lexical form
     * @param lang the language tag
     * @param node_storage optional custom node_storage used to store the literal
     */
    [[nodiscard]] static Literal make_lang_tagged_normalize(std::string_view lexical_form, std::string_view lang_tag,
                                                            NodeStorage &node_storage = NodeStorage::default_instance());
    /**
     * Constructs a Literal from a lexical form and a datatype.
     * @param lexical_form the lexical form
     * @param datatype the datatype
     * @param node_storage optional custom node_storage used to store the literal
     * @throws std::runtime_error if lexical_form contains invalid unicode (only xsd::string)
     */
    [[nodiscard]] static Literal make_typed(std::string_view lexical_form, IRI const &datatype,
                                            NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs a Literal from a lexical form and a datatype provided as a template parameter.
     * @tparam lexical_form the lexical form
     * @param T the datatype
     * @param node_storage optional custom node_storage used to store the literal
     * @throws std::runtime_error if lexical_form contains invalid unicode (only xsd::string)
     */
    template<datatypes::LiteralDatatype T>
    [[nodiscard]] static Literal make_typed(std::string_view lexical_form,
                                            NodeStorage &node_storage = NodeStorage::default_instance()) {

        if constexpr (std::is_same_v<T, datatypes::rdf::LangString>) {
            // see: https://www.w3.org/TR/rdf11-concepts/#section-Graph-Literal
            throw std::invalid_argument{"cannot construct rdf:langString without a language tag, please call one of the other factory functions"};
        } else if constexpr (std::is_same_v<T, datatypes::xsd::String>) {
            return Literal::make_simple(lexical_form, node_storage);
        }

        auto value = T::from_string(lexical_form);

        if constexpr (datatypes::IsInlineable<T>) {
            if (auto const maybe_inlined = T::try_into_inlined(value); maybe_inlined.has_value()) {
                return Literal::make_inlined_typed_unchecked(*maybe_inlined, T::fixed_id, node_storage);
            }
        }

        if constexpr (datatypes::HasFixedId<T>) {
            if (node_storage.has_specialized_storage_for(T::fixed_id)) {
                return Literal::make_noninlined_special_unchecked(std::any{std::move(value)}, T::fixed_id, node_storage);
            }
        }

        auto const lex = T::to_canonical_string(value);
        auto const needs_escape = lexical_form_needs_escape(lex);
        return Literal::make_noninlined_typed_unchecked(lex,
                                                        needs_escape,
                                                        IRI{T::identifier, node_storage},
                                                        node_storage);
    }

    /**
     * Constructs a literal from a compatible type. In this version of the function the datatype is specified at compile time.
     * No runtime lookup of the type information is required.
     * If type information is available at compile time, you should use this version of the function.
     *
     * @tparam T the datatype
     * @param compatible_value instance for which the literal is created
     * @param node_storage NodeStorage used
     * @throws std::runtime_error if lexical_form contains invalid unicode (only xsd::string)
     * @return literal instance representing compatible_value
     */
    template<datatypes::LiteralDatatype T>
    [[nodiscard]] static Literal make_typed_from_value(typename T::cpp_type const &compatible_value,
                                                       NodeStorage &node_storage = NodeStorage::default_instance()) noexcept {

        if constexpr (std::is_same_v<T, datatypes::rdf::LangString>) {
            return Literal::make_lang_tagged(compatible_value.lexical_form,
                                             compatible_value.language_tag,
                                             node_storage);
        }

        if constexpr (std::is_same_v<T, datatypes::xsd::String>) {
            return Literal::make_simple(compatible_value, node_storage);
        }

        if constexpr (datatypes::IsInlineable<T>) {
            if (auto const maybe_inlined = T::try_into_inlined(compatible_value); maybe_inlined.has_value()) {
                return Literal::make_inlined_typed_unchecked(*maybe_inlined, T::fixed_id, node_storage);
            }
        }

        if constexpr (datatypes::HasFixedId<T>) {
            if (node_storage.has_specialized_storage_for(T::fixed_id)) {
                return Literal{NodeBackendHandle{node_storage.find_or_make_id(storage::node::view::ValueLiteralBackendView{
                                                         .datatype = T::fixed_id,
                                                         .value = std::any{compatible_value}}),
                                        storage::node::identifier::RDFNodeType::Literal, node_storage.id()}};
            }
        }

        auto const lex = T::to_canonical_string(compatible_value);
        auto const needs_escape = lexical_form_needs_escape(lex);

        return Literal::make_noninlined_typed_unchecked(lex,
                                                        needs_escape,
                                                        IRI{T::datatype_id, node_storage},
                                                        node_storage);
    }

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
     * creates a new string Literal containing a random UUID (Universally Unique IDentifier)
     * @return UUID Literal
     */
    [[nodiscard]] static Literal make_string_uuid(NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Generates a random double in the range [0.0, 1.0).
     * The values are generated in a thread-safe manner using a lazily initialized thread_local random generator.
     *
     * @see https://www.w3.org/TR/sparql11-query/#idp2130040
     */
    [[nodiscard]] static Literal generate_random_double(NodeStorage &node_storage = NodeStorage::default_instance());

    Literal to_node_storage(NodeStorage &node_storage) const noexcept;
    [[nodiscard]] Literal try_get_in_node_storage(NodeStorage const &node_storage) const noexcept;

    /**
     * Checks if the datatype of this matches the provided LiteralDatatype
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @tparam T the datatype to compare against
     * @return true iff this datatype is T
     */
    template<datatypes::LiteralDatatype T>
    [[nodiscard]] bool datatype_eq() const noexcept {
        if constexpr (datatypes::HasFixedId<T>) {
            if (auto const type = this->handle_.node_id().literal_type(); type.is_fixed()) {
                return type == T::fixed_id;
            }

            return false;
        }

        return this->dynamic_datatype_eq_impl(T::identifier);
    }

    /**
     * Checks if the datatype of this matches the given IRI
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @param datatype the datatype to compare against
     * @return true iff this datatype is datatype
     */
    [[nodiscard]] bool datatype_eq(IRI const &datatype) const noexcept;

    /**
     * Checks if the datatype of this matches the datatype of other
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @param other other literal to check against
     * @return true iff this' datatype matches other's datatype
     */
    [[nodiscard]] bool datatype_eq(Literal const &other) const noexcept;

    /**
     * Checks if the datatype of this matches the provided LiteralDatatype
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @tparam T the datatype to compare against
     * @return true as xsd:boolean iff this datatype is T or null-literal if this is null
     */
    template<datatypes::LiteralDatatype T>
    [[nodiscard]] Literal as_datatype_eq(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept {
        if (this->null()) {
            return Literal{};
        }

        return Literal::make_boolean(this->datatype_eq<T>(), node_storage);
    }

    /**
     * Checks if the datatype of this matches the given IRI
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @param datatype the datatype to compare against
     * @return true as xsd:boolean iff this datatype is datatype or null-literal if this is null
     */
    [[nodiscard]] Literal as_datatype_eq(IRI const &datatype, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * Checks if the datatype of this matches the datatype of other
     * @note You should prefer this function over comparing datatypes using Literal::datatype
     *
     * @param other other literal to check against
     * @return true as xsd:boolean iff this' datatype matches other's datatype or null-literal if this or other is null
     */
    [[nodiscard]] Literal as_datatype_eq(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

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
    template<datatypes::LiteralDatatype T>
    [[nodiscard]] Literal cast(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept {
        return this->cast(IRI{T::datatype_id, node_storage}, node_storage);
    }

    /**
     * Tries to cast this literal to a literal of the given type and return the result without creating a Literal.
     *
     * @return conversion result
     */
    template<datatypes::LiteralDatatype T>
    requires (!std::same_as<T, datatypes::xsd::String>)
    std::optional<typename T::cpp_type> cast_to_value() const noexcept {
        using namespace datatypes::registry;
        using namespace datatypes::xsd;

        if (this->null()) {
            return std::nullopt;
        }

        auto const this_dtid = this->datatype_id();
        DatatypeIDView const target_dtid = T::datatype_id;

        if (this_dtid == target_dtid) {
            return this->value<T>();
        }

        if (this_dtid == String::datatype_id) {
            // string -> any
            try {
                return T::from_string(this->lexical_form());
            } catch (...) {
                return std::nullopt;
            }
        }

        if constexpr (std::same_as<T, Boolean>) {
            // any -> bool
            rdf::util::TriBool t = this->ebv();
            if (t == rdf::util::TriBool::Err)
                return std::nullopt;
            else if (t == rdf::util::TriBool::True)
                return true;
            else
                return false;
        }

        auto const *target_e = DatatypeRegistry::get_entry(target_dtid);
        if (target_e == nullptr) {
            // target not registered
            return std::nullopt;
        }

        if (this_dtid == Boolean::datatype_id && target_e->numeric_ops.has_value()) {
            // bool -> numeric
            if (target_e->numeric_ops->is_impl()) {
                auto value = this->template value<Boolean>() ? target_e->numeric_ops->get_impl().one_value_fptr()
                                                             : target_e->numeric_ops->get_impl().zero_value_fptr();

                return std::any_cast<typename T::cpp_type>(value);
            } else {
                auto const &impl_converter = DatatypeRegistry::get_numeric_op_impl_conversion(*target_e);
                auto const *target_num_impl = DatatypeRegistry::get_numerical_ops(impl_converter.target_type_id);
                assert(target_num_impl != nullptr);

                // perform conversion as impl numeric type
                auto const value = this->template value<Boolean>() ? target_num_impl->get_impl().one_value_fptr()
                                                                   : target_num_impl->get_impl().zero_value_fptr();

                // downcast to target
                auto target_value = impl_converter.inverted_convert(value);

                if (!target_value.has_value()) {
                    // not representable as target type
                    return std::nullopt;
                }

                return std::any_cast<typename T::cpp_type>(*target_value);
            }
        }

        auto const *this_e = DatatypeRegistry::get_entry(this_dtid);
        if (this_e == nullptr) {
            // this datatype not registered
            return std::nullopt;
        }

        if (auto const common_conversion = DatatypeRegistry::get_common_type_conversion(this_e->conversion_table, target_e->conversion_table); common_conversion.has_value()) {
            // general cast
            // TODO: if performance is bad split into separate cases for up-, down- and cross-casting to avoid one set of std::any wrapping and unwrapping for the former 2

            auto const common_type_value = common_conversion->convert_lhs(this->value()); // upcast to common
            auto target_value = common_conversion->inverted_convert_rhs(common_type_value); // downcast to target
            if (!target_value.has_value()) {
                // downcast failed
                return std::nullopt;
            }
            return std::any_cast<typename T::cpp_type>(*target_value);
        }

        // no conversion found
        return std::nullopt;
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
     * Returns the language tag of this Literal. If the string is empty this has no language tag.
     * @return language tag
     */
    [[nodiscard]] std::string_view language_tag() const noexcept;

    /**
     * @return the language tag of this Literal as xsd:string. If the string is empty this has no language tag.
     */
    [[nodiscard]] Literal as_language_tag(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @param lang_tag language tag to compare against
     * @return if this->language_tag() == lang_tag or error if this is not langString
     */
    [[nodiscard]] util::TriBool language_tag_eq(std::string_view lang_tag) const noexcept;

    /**
     * @param other literal to compare against
     * @return if this->language_tag() == other.language_tag() or error if:
     *      - this is not rdf:langString
     *      - other is not rdf:langString
     */
    [[nodiscard]] util::TriBool language_tag_eq(Literal const &other) const noexcept;

    /**
     * @param lang_tag language tag to compare against
     * @return if this->language_tag() == lang_tag or null-literal if this is not langString
     */
    [[nodiscard]] Literal as_language_tag_eq(std::string_view lang_tag, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @param other literal to compare against
     * @return if this->language_tag() == other.language_tag() or null-literal if:
     *      - this is not rdf:langString
     *      - other is not rdf:langString
     */
    [[nodiscard]] Literal as_language_tag_eq(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;


    [[nodiscard]] explicit operator std::string() const noexcept;
    friend std::ostream &operator<<(std::ostream &os, const Literal &literal);

    /**
     * Constructs a datatype specific container from Literal.
     * @return std::any wrapped value. will be empty if type is not registered.
     */
    [[nodiscard]] std::any value() const noexcept;

    /**
     * Get the value of an literal. T must be the registered datatype for the datatype iri.
     * @tparam T datatype of the returned instance
     * @return T instance with the value from this
     */
    template<datatypes::LiteralDatatype T>
    typename T::cpp_type value() const {
        if (!this->datatype_eq<T>()) [[unlikely]] {
            throw std::runtime_error{"Literal::value error: incompatible type"};
        }

        if constexpr (std::is_same_v<T, datatypes::xsd::String>) {
            auto const view = this->handle_.literal_backend();
            auto const &lit = view.get_lexical();

            return lit.lexical_form;
        }

        if constexpr (std::is_same_v<T, datatypes::rdf::LangString>) {
            auto const handle = this->is_inlined()
                    ? this->lang_tagged_get_de_inlined().backend_handle()
                    : this->backend_handle();

            auto const view = handle.literal_backend();
            auto const &lit = view.get_lexical();

            return datatypes::registry::LangStringRepr{.lexical_form = lit.lexical_form,
                                                       .language_tag = lit.language_tag};
        }

        if constexpr (datatypes::IsInlineable<T>) {
            if (this->is_inlined()) {
                auto const inlined_value = this->handle_.node_id().literal_id();
                return T::from_inlined(inlined_value);
            }
        }

        auto const backend = handle_.literal_backend();
        return backend.visit(
                [](storage::node::view::LexicalFormLiteralBackendView const &lexical) noexcept {
                    return T::from_string(lexical.lexical_form);
                },
                [](storage::node::view::ValueLiteralBackendView const &any) noexcept {
                    assert(any.datatype == T::datatype_id);
                    return std::any_cast<typename T::cpp_type>(any.value);
                });
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
    [[nodiscard]] Literal as_eq(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    util::TriBool operator==(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool ne(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_ne(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    util::TriBool operator!=(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool lt(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_lt(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    util::TriBool operator<(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool le(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_le(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    util::TriBool operator<=(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool gt(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_gt(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    util::TriBool operator>(Literal const &other) const noexcept;

    [[nodiscard]] util::TriBool ge(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_ge(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    util::TriBool operator>=(Literal const &other) const noexcept;

    [[nodiscard]] bool eq_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_eq_with_extensions(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] bool ne_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_ne_with_extensions(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] bool lt_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_lt_with_extensions(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] bool le_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_le_with_extensions(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] bool gt_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_gt_with_extensions(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    [[nodiscard]] bool ge_with_extensions(Literal const &other) const noexcept;
    [[nodiscard]] Literal as_ge_with_extensions(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    [[nodiscard]] Literal add(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator+(Literal const &other) const noexcept;
    Literal &add_assign(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) noexcept;
    Literal &operator+=(Literal const &other) noexcept;

    [[nodiscard]] Literal sub(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator-(Literal const &other) const noexcept;
    Literal &sub_assign(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) noexcept;
    Literal &operator-=(Literal const &other) noexcept;

    [[nodiscard]] Literal mul(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator*(Literal const &other) const noexcept;
    Literal &mul_assign(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) noexcept;
    Literal &operator*=(Literal const &other) noexcept;

    [[nodiscard]] Literal div(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator/(Literal const &other) const noexcept;
    Literal &div_assign(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) noexcept;
    Literal &operator/=(Literal const &other) noexcept;

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
    [[nodiscard]] util::TriBool language_tag_matches_range(std::string_view lang_range) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-string-length
     * @param lang_range a basic language range as xsd:string
     * @return whether the language tag of this matches the given lang range or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal as_language_tag_matches_range(std::string_view lang_range, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-string-length
     * @param lang_range a basic language range as xsd:string
     * @return whether the language tag of this matches the given lang range or the null literal if
     *      - this is not string-like
     *      - lang_range is not xsd:string
     */
    [[nodiscard]] Literal as_language_tag_matches_range(Literal const &lang_range, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

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
    [[nodiscard]] Literal as_regex_matches(Literal const &pattern, Literal const &flags = Literal::make_simple(""), NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

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
                                        Literal const &flags = Literal::make_simple(""),
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
     * @see https://www.w3.org/TR/xpath-functions/#func-encode-for-uri
     * @param string literal to encode
     * @return URL encoded string type literal
     */
    [[nodiscard]] static Literal encode_for_uri(std::string_view string, NodeStorage &node_storage = NodeStorage::default_instance());
    /**
     * @see https://www.w3.org/TR/xpath-functions/#func-encode-for-uri
     * @return URL encoded string type literal or the null literal if
     *      - this is not string-like
     */
    [[nodiscard]] Literal encode_for_uri(NodeStorage &node_storage = NodeStorage::default_instance()) const;

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
                                 Literal const &len = Literal::make_typed_from_value<datatypes::xsd::Double>(std::numeric_limits<datatypes::xsd::Double::cpp_type>::infinity()),
                                 NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

private:
    /**
     * hash via OpenSSL
     * @param alg OpenSSL hash algorithm identifier (see https://www.openssl.org/docs/man1.1.1/man3/EVP_Digest.html for all available hash algorithms)
     * @return hash as simple literal, or null literal if this is not of type xsd::String
     */
    [[nodiscard]] Literal hash_with(const char *alg, NodeStorage &node_storage) const;

public:
    /**
     * @see https://www.w3.org/TR/sparql11-query/#func-md5
     * @return MD5 hash as simple literal, or null literal if this is not of type xsd::String
     */
    [[nodiscard]] Literal md5(NodeStorage &node_storage = NodeStorage::default_instance()) const;
    /**
     * @see https://www.w3.org/TR/sparql11-query/#func-sha1
     * @return SHA1 hash as simple literal, or null literal if this is not of type xsd::String
     */
    [[nodiscard]] Literal sha1(NodeStorage &node_storage = NodeStorage::default_instance()) const;
    /**
     * @see https://www.w3.org/TR/sparql11-query/#func-sha256
     * @return SHA2-256 hash as simple literal, or null literal if this is not of type xsd::String
     */
    [[nodiscard]] Literal sha256(NodeStorage &node_storage = NodeStorage::default_instance()) const;
    /**
     * @see https://www.w3.org/TR/sparql11-query/#func-sha384
     * @return SHA2-384 hash as simple literal, or null literal if this is not of type xsd::String
     */
    [[nodiscard]] Literal sha384(NodeStorage &node_storage = NodeStorage::default_instance()) const;

    /**
     * @see https://www.w3.org/TR/sparql11-query/#func-sha512
     * @return SHA2-512 hash as simple literal, or null literal if this is not of type xsd::String
     */
    [[nodiscard]] Literal sha512(NodeStorage &node_storage = NodeStorage::default_instance()) const;

    /**
     * returns the current time.
     * Note: will need to be buffered for each query, because each query has only one now.
     * @return std::chrono::system_clock::now() as xsd:dateTime
     */
    [[nodiscard]] static Literal now(NodeStorage &node_storage = NodeStorage::default_instance()) noexcept;

    /**
     * returns the year part of this.
     * @return year or nullopt
     */
    [[nodiscard]] std::optional<std::chrono::year> year() const noexcept;
    /**
     * returns the year part of this.
     * @return xsd::Integer or null literal
     */
    [[nodiscard]] Literal as_year(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the month part of this.
     * @return month or nullopt
     */
    [[nodiscard]] std::optional<std::chrono::month> month() const noexcept;
    /**
     * returns the month part of this.
     * @return xsd::Integer or null literal
     */
    [[nodiscard]] Literal as_month(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the day part of this.
     * @return day or nullopt
     */
    [[nodiscard]] std::optional<std::chrono::day> day() const noexcept;
    /**
     * returns the day part of this.
     * @return xsd::Integer or null literal
     */
    [[nodiscard]] Literal as_day(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the hours part of this.
     * @return hours ot nullopt
     */
    [[nodiscard]] std::optional<std::chrono::hours> hours() const noexcept;
    /**
     * returns the hours part of this.
     * @return xsd::Integer or null literal
     */
    [[nodiscard]] Literal as_hours(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the minutes part of this.
     * @return minutes ot nullopt
     */
    [[nodiscard]] std::optional<std::chrono::minutes> minutes() const noexcept;
    /**
     * returns the minutes part of this.
     * @return xsd::Integer or null literal
     */
    [[nodiscard]] Literal as_minutes(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the seconds (including fractional) part of this.
     * @return seconds or nullopt
     */
    [[nodiscard]] std::optional<std::chrono::milliseconds> seconds() const noexcept;
    /**
     * returns the seconds (including fractional) part of this.
     * @return xsd::Decimal or null literal
     */
    [[nodiscard]] Literal as_seconds(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the timezone offset part of this.
     * @return timezone or nullopt
     */
    [[nodiscard]] std::optional<util::Timezone> timezone() const noexcept;
    /**
     * returns the timezone offset part of this.
     * @return offset as xsd::DayTimeDuration or null literal
     */
    [[nodiscard]] Literal as_timezone(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * returns the timezone offset part of this.
     * @return timezone as string or nullopt
     */
    [[nodiscard]] std::optional<std::string> tz() const noexcept;
    /**
     * returns the timezone offset part of this.
     * @return timezone as simple literal or null literal
     */
    [[nodiscard]] Literal as_tz(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * @return the effective boolean value of this
     */
    [[nodiscard]] util::TriBool ebv() const noexcept;

    /**
     * Converts this literal to its effective boolean value
     * @return Literal containing the ebv
     */
    [[nodiscard]] Literal as_ebv(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    [[nodiscard]] Literal logical_and(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator&&(Literal const &other) const noexcept;

    [[nodiscard]] Literal logical_or(Literal const &other, NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator||(Literal const &other) const noexcept;

    [[nodiscard]] Literal logical_not(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;
    Literal operator!() const noexcept;

    friend class Node;
};

/**
 * normalizes a UTF-8 string to NFC
 * @param utf8
 * @return normalized string
 */
[[nodiscard]] std::string normalize_unicode(std::string_view utf8);

/**
 * @return whether lang_tag matches the basic language range lang_range
 */
[[nodiscard]] bool lang_matches(std::string_view lang_tag, std::string_view lang_range) noexcept;

/**
 * @return whether lang_tag matches the basic language range lang_range as xsd:boolean or the null-literal if:
 *      - lang_tag is not xsd:string
 *      - lang_range is not xsd:string
 */
[[nodiscard]] Literal lang_matches(Literal const &lang_tag, Literal const &lang_range, storage::node::NodeStorage &node_storage = storage::node::NodeStorage::default_instance()) noexcept;

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
