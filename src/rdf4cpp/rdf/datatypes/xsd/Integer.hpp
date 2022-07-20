
/**
 * @file Registers xsd:integer with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INTEGER_HPP
#define RDF4CPP_XSD_INTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

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

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<xsd_integer>::cpp_type capabilities::Default<xsd_integer>::from_string(std::string_view s) {

    const std::regex integer_regex("[\\-+]?[0-9]+");

    if (std::regex_match(s.data(), integer_regex)) {
        return std::strtol(s.data(), nullptr, 10);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

template<>
inline bool capabilities::Logical<xsd_integer>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::integer
 */
using Integer = registry::LiteralDatatypeImpl<registry::xsd_integer,
                                              registry::capabilities::Logical,
                                              registry::capabilities::Numeric,
                                              registry::capabilities::Subtype>;
}  // namespace rdf4cpp::rdf::datatypes::xsd
#endif  //RDF4CPP_XSD_INTEGER_HPP
