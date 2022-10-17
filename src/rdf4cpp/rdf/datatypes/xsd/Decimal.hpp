
/**
* @file Registers xsd:decimal with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_DECIMAL_HPP
#define RDF4CPP_XSD_DECIMAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Float.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <charconv>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_decimal> {
    // needs at least 18 decimal digits of precision
    // see: https://www.w3.org/TR/2004/REC-xmlschema-2-20041028/#dt-decimal
    using cpp_datatype = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<18, int16_t>>;
};

template<>
struct DatatypePromotionMapping<xsd_decimal> {
    using promoted = xsd::Float;
};

template<>
inline capabilities::Default<xsd_decimal>::cpp_type capabilities::Default<xsd_decimal>::from_string(std::string_view s) {
    static std::regex const decimal_regex{R"#((\+|-)?[0-9]+\.[0-9]*)#"};

    if (!std::regex_match(s.begin(), s.end(), decimal_regex)) {
        throw std::runtime_error{"XSD Parsing Error"};
    }

    if (s.starts_with('+')) {
        s.remove_prefix(1);
    }

    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"xsd:decimal parsing error: "} + e.what()};
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string capabilities::Default<xsd_decimal>::to_string(const cpp_type &value) {

    cpp_type int_part, fract_part;
    fract_part = modf(value, &int_part);
    bool remove_trailing_zeros = false;
    std::ostringstream str_os;
    str_os << std::fixed;
    if (fract_part == 0) {
        //If the incoming value is a whole number (no fractional part) then precision is set to 1 to have a decimal representation (50 -> 50.0)
        str_os << std::setprecision(1);
    } else {
        //If the incoming value has a fractional part which has a value greater than zero, then maximum precision is set, to convert it to the nearest possible representation
        str_os << std::setprecision(std::numeric_limits<cpp_type>::digits10);
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
inline nonstd::expected<capabilities::Numeric<xsd_decimal>::div_result_cpp_type, NumericOpError> capabilities::Numeric<xsd_decimal>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#func-numeric-divide
    // decimal needs error (and cpp_type is not integral)

    if (rhs == 0) {
        return nonstd::make_unexpected(NumericOpError::DivideByZero);
    }

    return lhs / rhs;
}

template<>
inline bool capabilities::Logical<xsd_decimal>::effective_boolean_value(cpp_type const &value) noexcept {
    return !isnan(value) && value != 0.0;
}

template<>
inline std::partial_ordering capabilities::Comparable<xsd_decimal>::compare(cpp_type const &lhs, cpp_type const &rhs) {
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
 * Implementation of xsd::decimal
 */
struct Decimal : registry::LiteralDatatypeImpl<registry::xsd_decimal,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Numeric,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::Promotable,
                                               registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DECIMAL_HPP
