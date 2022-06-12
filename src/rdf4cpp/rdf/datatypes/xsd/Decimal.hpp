
/**
* @file Registers xsd:decimal with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_DECIMAL_HPP
#define RDF4CPP_XSD_DECIMAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_decimal{"http://www.w3.org/2001/XMLSchema#decimal"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_decimal> {
    using cpp_datatype = double;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_decimal>::cpp_type LiteralDatatypeImpl<xsd_decimal>::from_string(std::string_view s) {

    const std::regex decimal_regex("(\\+|-)?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)");

    if (std::regex_match(s.data(), decimal_regex)) {
        return std::strtod(s.data(), nullptr);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_decimal>::to_string(const cpp_type &value) {

    std::ostringstream str_os;
    // Set Fixed -Point Notation for Decimal
    str_os << std::fixed << value;
    std::string str = str_os.str();
    return str;
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::decimal
 */
using Decimal = registry::LiteralDatatypeImpl<registry::xsd_decimal>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DECIMAL_HPP
