#ifndef RDF4CPP_LITERALDATATYPEIMPL_HPP
#define RDF4CPP_LITERALDATATYPEIMPL_HPP

#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeRegistry.hpp>

#include <cstddef>
#include <iosfwd>
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

} // namespace detail

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
        // If not further specified, to_string is instanciated via operator<<. If operator<< is not defined for cpp_type instanciation will fail.
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
    static constexpr ConstexprString promoted_identifier = DatatypePromotionMapping<type_iri>::promoted_identifier;

    using cpp_type = typename DatatypeMapping<type_iri>::cpp_datatype;
    using promoted_cpp_type = typename DatatypeMapping<promoted_identifier>::cpp_datatype;

    static constexpr unsigned rank = detail_rank::DatatypePromotionRank<type_iri>::value;

    inline static promoted_cpp_type promote(cpp_type const &value) {
        return static_cast<promoted_cpp_type>(value);
    }
};

/**
 * The capability to be converted to it's supertype, e.g. Int -> Integer.
 */
template<ConstexprString type_iri>
struct Subtype {
    static constexpr ConstexprString supertype_identifier = DatatypeSupertypeMapping<type_iri>::supertype_identifier;

    using cpp_type = typename DatatypeMapping<type_iri>::cpp_datatype;
    using super_cpp_type = typename DatatypeMapping<supertype_identifier>::cpp_datatype;

    inline static super_cpp_type into_supertype(cpp_type const &value) {
        return static_cast<super_cpp_type>(value);
    }
};

/**
 * The capability to be used in numeric operations.
 */
template<ConstexprString xsd_string>
struct Numeric {
    using cpp_type = typename DatatypeMapping<xsd_string>::cpp_datatype;

    inline static cpp_type add(cpp_type const &lhs, cpp_type const &rhs) {
        return lhs + rhs;
    }

    inline static cpp_type sub(cpp_type const &lhs, cpp_type const &rhs) {
        return lhs - rhs;
    }

    inline static cpp_type mul(cpp_type const &lhs, cpp_type const &rhs) {
        return lhs * rhs;
    }

    inline static cpp_type div(cpp_type const &lhs, cpp_type const &rhs) {
        return lhs / rhs;
    }

    inline static cpp_type pos(cpp_type const &operand) {
        return +operand;
    }

    inline static cpp_type neg(cpp_type const &operand) {
        return -operand;
    }
};

/**
 * The capability to be used in boolean contexts.
 */
template<ConstexprString xsd_string>
struct Logical {
    using cpp_type = typename DatatypeMapping<xsd_string>::cpp_datatype;

    /**
     * @return the EBV (effective boolean value) of a datatype
     */
    inline static bool effective_boolean_value(cpp_type const &) {
        static_assert(detail::always_false_v<cpp_type>, "'effective_boolean_value' is not implemented for this type!");
    }
};

} // namespace capabilities

/**
 * An automatically registering LiteralDatatype with given capabilities.
 *
 * @tparam Capabilities all capabilities this instantiation should have
 */
template<ConstexprString type_iri_t, template<ConstexprString> typename ...Capabilities>
struct LiteralDatatypeImpl : capabilities::Default<type_iri_t>, Capabilities<type_iri_t>... {
    using typename capabilities::Default<type_iri_t>::cpp_type;

private:
    inline static std::nullptr_t init();
    inline static const auto dummy = init();

    // Force `dummy` to be instantiated, even though it's unused.
    static constexpr std::integral_constant<decltype(&dummy), &dummy> dummy_helper{};
};

template<ConstexprString type_iri, template<ConstexprString> typename ...Capabilities>
std::nullptr_t LiteralDatatypeImpl<type_iri, Capabilities...>::init() {
    DatatypeRegistry::add<LiteralDatatypeImpl<type_iri, Capabilities...>>();
    return nullptr;
}

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_LITERALDATATYPEIMPL_HPP
