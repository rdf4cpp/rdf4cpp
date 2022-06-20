
/**
* @file Registers xsd:decimal with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_DECIMAL_HPP
#define RDF4CPP_XSD_DECIMAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <regex>
#include <iostream>

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

    double int_part, fract_part;
    fract_part  = modf(value, &int_part);
    bool remove_trailing_zeros = false;
    std::ostringstream str_os;
    str_os << std::fixed;
    if (fract_part == 0) {
        //If the incoming value is a whole number (no fractional part) then precision is set to 1 to have a decimal representation (50 -> 50.0)
        str_os << std::setprecision(1);
    }
    else {
        //If the incoming value has a fractional part which has a value greater than zero, then maximum precision is set, to convert it to nearest possible representation
        str_os << std::setprecision(std::numeric_limits<double>::max_digits10 + 2);
        remove_trailing_zeros = true;
    }
    str_os << value;
    std::string str = str_os.str();

    //Removes trailing zeros from fractional part if precision was set to maximum
    if(remove_trailing_zeros && str.find('.') != std::string::npos)
    {
        //Removes trailing zeroes
        str = str.substr(0, str.find_last_not_of('0')+1);
    }
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
