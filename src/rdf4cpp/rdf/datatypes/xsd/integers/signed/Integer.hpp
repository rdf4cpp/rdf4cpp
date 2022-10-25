
/**
 * @file Registers xsd:integer with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INTEGER_HPP
#define RDF4CPP_XSD_INTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>

#include <stdexcept>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_integer> {
    using cpp_datatype = boost::multiprecision::cpp_int;
};

template<>
struct DatatypeSupertypeMapping<xsd_integer> {
    using supertype = xsd::Decimal;
};

template<>
struct DatatypeDivResultMapping<xsd_integer> {
    using op_result = xsd::Decimal;
};

template<>
inline capabilities::Default<xsd_integer>::cpp_type capabilities::Default<xsd_integer>::from_string(std::string_view s) {
    if (s.starts_with('+')) {
        s.remove_prefix(1);
    }

    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"xsd:integer parsing error: "} + e.what()};
    }
}

template<>
inline bool capabilities::Logical<xsd_integer>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
inline nonstd::expected<capabilities::Numeric<xsd_integer>::div_result_cpp_type, NumericOpError> capabilities::Numeric<xsd_integer>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (rhs == 0) {
        return nonstd::make_unexpected(NumericOpError::DivideByZero);
    }

    // https://www.w3.org/TR/xpath-functions/#func-numeric-divide
    // integer needs to return decimal on division
    return static_cast<div_result_cpp_type>(lhs) / static_cast<div_result_cpp_type>(rhs);
}

template<>
inline std::partial_ordering capabilities::Comparable<xsd_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::integer
 */
struct Integer : registry::LiteralDatatypeImpl<registry::xsd_integer,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Numeric,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::Subtype,
                                               registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd
#endif  //RDF4CPP_XSD_INTEGER_HPP
