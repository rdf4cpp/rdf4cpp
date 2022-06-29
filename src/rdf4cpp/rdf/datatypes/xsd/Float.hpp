/**
 * @file Registers xsd:float with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_FLOAT_HPP
#define RDF4CPP_XSD_FLOAT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <math.h>
#include <ostream>
#include <regex>
namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_float{"http://www.w3.org/2001/XMLSchema#float"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_float> {
    using cpp_datatype = float;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_float>::cpp_type LiteralDatatypeImpl<xsd_float>::from_string(std::string_view s) {
    const std::regex float_regex("(\\+|-)?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)([Ee](\\+|-)?[0-9]+)?|(\\+|-)?INF|NaN");

    if (std::regex_match(s.data(), float_regex)) {
        auto ret = std::strtof(s.data(), nullptr);
        return ret;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_float>::to_string(const cpp_type &value) {

    if (isnan(value)) {
        //checks whether value is "nan", then returns rdf representation of nan
        return "NaN";
    } else if (value == -std::numeric_limits<float>::infinity()) {
        //checks whether value is "-inf", then returns rdf representation of -inf
        return "-INF";
    } else if (value == std::numeric_limits<float>::infinity()) {
        //checks whether value is "inf", then returns rdf representation of inf
        return "INF";
    } else {
        //If value is a float number (not inf or nan)
        float int_part, fract_part;
        fract_part = modf(value, &int_part);
        bool remove_trailing_zeros = false;
        std::ostringstream str_os;
        str_os << std::fixed;
        if (fract_part == 0) {
            //If the incoming value is a whole number (no fractional part) then precision is set to 1 to have a decimal representation (50 -> 50.0)
            str_os << std::setprecision(1);
        } else {
            //If the incoming value has a fractional part which has a value greater than zero, then maximum precision is set, to convert it to nearest possible representation
            str_os << std::setprecision(38);
            remove_trailing_zeros = true;
        }
        str_os << value;
        std::string str = str_os.str();

        //Removes trailing zeros from fractional part if precision was set to maximum
        if (remove_trailing_zeros && str.find('.') != std::string::npos) {
            //Removes trailing zeroes
            str = str.substr(0, str.find_last_not_of('0') + 1);
        }
        return str;
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::float
 */
using Float = registry::LiteralDatatypeImpl<registry::xsd_float>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_FLOAT_HPP
