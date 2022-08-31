#ifndef RDF4CPP_LITERALDATATYPEIMPL_HPP
#define RDF4CPP_LITERALDATATYPEIMPL_HPP

#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeRegistry.hpp>

#include <cstddef>
#include <sstream>
#include <string_view>
#include <type_traits>

namespace rdf4cpp::rdf::datatypes::registry {

namespace capabilities {

namespace detail {

/**
 * static_assert would always trigger if it wasn't dependent on a template parameter.
 * With this helper template, it only triggers if the function is instantiated.
 */
template<typename>
using always_false = std::false_type;
template<typename T>
static constexpr bool always_false_v = always_false<T>::value;

template<typename OpResMapping, typename Fallback, typename enable = void>
struct SelectOpResult {
    using type = typename OpResMapping::cpp_type;
};

template<typename Fallback>
struct SelectOpResult<std::false_type, Fallback> {
    using type = Fallback;
};

}  // namespace detail

/**
 * The default capability. All LiteralDatatypes must implement these operations.
 */
template<ConstexprString type_iri_t>
struct Default {
    /**
     * IRI of the LiteralDatatype.
     */
    static constexpr ConstexprString identifier = type_iri_t;

    /**
     * The C++ type that this LiteralDatatype is mapped to.
     */
    using cpp_type = typename DatatypeMapping<identifier>::cpp_datatype;

    /**
     * Factory function that parses a string representing datatype_t and builds an instance of datatype_t
     * @return instance of datatype_t
     */
    inline static cpp_type from_string(std::string_view) {
        // If this implementation is used the user forgot to provide their own.
        static_assert(detail::always_false_v<cpp_type>, "'from_string' is not implemented for this type!");
    }
    /**
     * Returns string representation of a datatype_t.
     * @param value the value
     * @return <div>value</div>'s canonical string representation
     */
    inline static std::string to_string(cpp_type const &value) {
        // If not further specified, to_string is instantiated via operator<<. If operator<< is not defined for cpp_type instantiation will fail.
        std::stringstream str_s;
        str_s << value;
        return str_s.str();
    }
};

/**
 * The capability to be promoted to another LiteralDatatype, e.g. Decimal -> Float.
 */
template<ConstexprString type_iri>
struct Promotable {
    using promoted = typename DatatypePromotionMapping<type_iri>::promoted;

    using cpp_type = typename DatatypeMapping<type_iri>::cpp_datatype;
    using promoted_cpp_type = typename DatatypeMapping<promoted::identifier>::cpp_datatype;

    static constexpr unsigned promotion_rank = detail_rank::DatatypePromotionRank<type_iri>::value;

    inline static promoted_cpp_type promote(cpp_type const &value) noexcept {
        return static_cast<promoted_cpp_type>(value);
    }
};

/**
 * The capability to be converted to it's supertype, e.g. Int -> Integer.
 */
template<ConstexprString type_iri>
struct Subtype {
    using supertype = typename DatatypeSupertypeMapping<type_iri>::supertype;

    using cpp_type = typename DatatypeMapping<type_iri>::cpp_datatype;
    using super_cpp_type = typename DatatypeMapping<supertype::identifier>::cpp_datatype;

    static constexpr unsigned subtype_rank = detail_rank::DatatypeSubtypeRank<type_iri>::value;

    inline static super_cpp_type into_supertype(cpp_type const &value) noexcept {
        return static_cast<super_cpp_type>(value);
    }
};

/**
 * The capability to be used in numeric operations.
 */
template<ConstexprString type_iri>
struct Numeric {
    using cpp_type = typename DatatypeMapping<type_iri>::cpp_datatype;

    using add_result = typename DatatypeAddResultMapping<type_iri>::op_result;
    using sub_result = typename DatatypeSubResultMapping<type_iri>::op_result;
    using mul_result = typename DatatypeMulResultMapping<type_iri>::op_result;
    using div_result = typename DatatypeDivResultMapping<type_iri>::op_result;
    using pos_result = typename DatatypePosResultMapping<type_iri>::op_result;
    using neg_result = typename DatatypeNegResultMapping<type_iri>::op_result;

    using add_result_cpp_type = typename detail::SelectOpResult<add_result, cpp_type>::type;
    using sub_result_cpp_type = typename detail::SelectOpResult<sub_result, cpp_type>::type;
    using mul_result_cpp_type = typename detail::SelectOpResult<mul_result, cpp_type>::type;
    using div_result_cpp_type = typename detail::SelectOpResult<div_result, cpp_type>::type;
    using pos_result_cpp_type = typename detail::SelectOpResult<pos_result, cpp_type>::type;
    using neg_result_cpp_type = typename detail::SelectOpResult<neg_result, cpp_type>::type;

    inline static add_result_cpp_type add(cpp_type const &lhs, cpp_type const &rhs) noexcept {
        return lhs + rhs;
    }

    inline static sub_result_cpp_type sub(cpp_type const &lhs, cpp_type const &rhs) noexcept {
        return lhs - rhs;
    }

    inline static mul_result_cpp_type mul(cpp_type const &lhs, cpp_type const &rhs) noexcept {
        return lhs * rhs;
    }

    inline static div_result_cpp_type div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
        return lhs / rhs;
    }

    inline static pos_result_cpp_type pos(cpp_type const &operand) noexcept {
        return +operand;
    }

    inline static neg_result_cpp_type neg(cpp_type const &operand) noexcept {
        return -operand;
    }
};

/**
 * The capability to be used in boolean contexts.
 */
template<ConstexprString type_iri>
struct Logical {
    using cpp_type = typename DatatypeMapping<type_iri>::cpp_datatype;

    /**
     * @return the EBV (effective boolean value) of a datatype
     */
    inline static bool effective_boolean_value(cpp_type const &) noexcept {
        static_assert(detail::always_false_v<cpp_type>, "'effective_boolean_value' is not implemented for this type!");
        return false;  // supress warnings in gcc, because it cannot properly detect that this is unreachable
    }
};

template<ConstexprString type_iri>
struct FixedId {
    static constexpr uint8_t fixed_id = DatatypeFixedIdMapping<type_iri>::fixed_id;
    static_assert(fixed_id > 1 && fixed_id < 64, "fixed id can only have 6 bits and the 0 and 1 values are reserved");
    // todo: figure out how to not reserve them as they are not useful
};

}  // namespace capabilities

/**
 * An automatically registering LiteralDatatype with given capabilities.
 *
 * @tparam Capabilities all capabilities this instantiation should have
 */
template<ConstexprString type_iri, template<ConstexprString> typename... Capabilities>
struct LiteralDatatypeImpl : capabilities::Default<type_iri>, Capabilities<type_iri>... {
    using typename capabilities::Default<type_iri>::cpp_type;

private:
    inline static std::nullptr_t init();
    inline static const auto dummy = init();

    // Force `dummy` to be instantiated, even though it's unused.
    static constexpr std::integral_constant<decltype(&dummy), &dummy> dummy_helper{};
};

template<ConstexprString type_iri, template<ConstexprString> typename... Capabilities>
std::nullptr_t LiteralDatatypeImpl<type_iri, Capabilities...>::init() {
    DatatypeRegistry::add<LiteralDatatypeImpl<type_iri, Capabilities...>>();
    return nullptr;
}

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_LITERALDATATYPEIMPL_HPP
