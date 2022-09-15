
/**
 * @file Registers xsd:integer with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INTEGER_HPP
#define RDF4CPP_XSD_INTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>

#include <charconv>
#include <cstdint>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_integer{"http://www.w3.org/2001/XMLSchema#integer"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_integer> {
    using cpp_datatype = int64_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_integer> {
    using supertype = xsd::Decimal;
};

template<>
struct DatatypeDivResultMapping<xsd_integer> {
    using op_result = xsd::Decimal;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<xsd_integer>::cpp_type capabilities::Default<xsd_integer>::from_string(std::string_view s) {

    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    cpp_type value;
    std::from_chars_result const res = std::from_chars(s.data(), s.data() + s.size(), value, 10);

    if (res.ptr != s.data() + s.size()) {
        // parsing did not reach end of string => it contains invalid characters
        throw std::runtime_error{"XSD Parsing Error"};
    }

    return value;
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

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::integer
 */
struct Integer : registry::LiteralDatatypeImpl<registry::xsd_integer,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Numeric,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::Subtype> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd
#endif  //RDF4CPP_XSD_INTEGER_HPP
