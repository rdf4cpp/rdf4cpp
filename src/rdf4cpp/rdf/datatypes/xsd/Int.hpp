/**
 * @file Registers xsd:int with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INT_HPP
#define RDF4CPP_XSD_INT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Integer.hpp>

#include <charconv>
#include <cstdint>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_int{"http://www.w3.org/2001/XMLSchema#int"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_int> {
    using cpp_datatype = int32_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_int> {
    using supertype = xsd::Integer;
};

template<>
struct DatatypeDivResultMapping<xsd_int> {
    using op_result = xsd::Decimal;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<xsd_int>::cpp_type capabilities::Default<xsd_int>::from_string(std::string_view s) {

    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error{"XSD Parsing Error"};
    } else {
        return value;
    }
}

template<>
inline bool capabilities::Logical<xsd_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
inline capabilities::Numeric<xsd_int>::div_result_cpp_type capabilities::Numeric<xsd_int>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return static_cast<div_result_cpp_type>(lhs) / static_cast<div_result_cpp_type>(rhs);
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::integer
 */
struct Int : registry::LiteralDatatypeImpl<registry::xsd_int,
                                           registry::capabilities::Logical,
                                           registry::capabilities::Numeric,
                                           registry::capabilities::Comparable,
                                           registry::capabilities::Subtype> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_INT_HPP
