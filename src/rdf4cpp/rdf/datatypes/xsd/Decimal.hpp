
/**
* @file Registers xsd:decimal with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_DECIMAL_HPP
#define RDF4CPP_XSD_DECIMAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Float.hpp>

#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static util::ConstexprString xsd_decimal{"http://www.w3.org/2001/XMLSchema#decimal"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_decimal> {
    using cpp_datatype = double;
};

template<>
struct DatatypePromotionMapping<xsd_decimal> {
    using promoted = xsd::Float;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<xsd_decimal>::cpp_type capabilities::Default<xsd_decimal>::from_string(std::string_view s) {

    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    cpp_type value;
    std::from_chars_result const res = std::from_chars(s.data(), s.data() + s.size(), value, std::chars_format::fixed);

    if (res.ptr != s.data() + s.size()) {
        // parsing did not reach end of string => it contains invalid characters
        throw std::runtime_error{"XSD Parsing Error"};
    }

    if (std::isnan(value) || std::isinf(value)) {
        // nan and +-inf not permitted in decimal
        throw std::runtime_error{"XSD Parsing Error"};
    }

    return value;
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string capabilities::Default<xsd_decimal>::to_string(const cpp_type &value) {

    double int_part, fract_part;
    fract_part = modf(value, &int_part);
    bool remove_trailing_zeros = false;
    std::ostringstream str_os;
    str_os << std::fixed;
    if (fract_part == 0) {
        //If the incoming value is a whole number (no fractional part) then precision is set to 1 to have a decimal representation (50 -> 50.0)
        str_os << std::setprecision(1);
    } else {
        //If the incoming value has a fractional part which has a value greater than zero, then maximum precision is set, to convert it to the nearest possible representation
        str_os << std::setprecision(std::numeric_limits<double>::max_digits10 + 2);
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

template<>
inline bool capabilities::Logical<xsd_decimal>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.0;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::decimal
 */
struct Decimal : registry::LiteralDatatypeImpl<registry::xsd_decimal,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Numeric,
                                               registry::capabilities::Promotable> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DECIMAL_HPP
