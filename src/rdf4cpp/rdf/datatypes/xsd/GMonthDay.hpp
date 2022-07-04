
/**
 * @file Registers xsd:gMonthDay with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_GMONTHDAY_HPP
#define RDF4CPP_XSD_GMONTHDAY_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>
#include <string>
#include <time.h>
namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_gMonthDay{"http://www.w3.org/2001/XMLSchema#gMonthDay"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_gMonthDay> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_gMonthDay>::cpp_type LiteralDatatypeImpl<xsd_gMonthDay>::from_string(std::string_view s) {

    const std::regex gMonthDay_regex("--(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])(Z|(\\+|-)((0[0-9]|1[0-3]):[0-5][0-9]|14:00))?");

    if (std::regex_match(s.data(), gMonthDay_regex)) {

        struct tm tm{};
        strptime(s.data(), "--%m-%d", &tm);

        //default values
        tm.tm_sec = 0; // seconds of minutes from 0 to 61
        tm.tm_min = 0; // minutes of hour from 0 to 59
        tm.tm_hour = 0; // hours of day from 0 to 24
        tm.tm_year = 0; // year since 1900
        tm.tm_isdst = -1; // value should be set even if not used

        return mktime(&tm);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_gMonthDay>::to_string(const cpp_type &value) {

    char str[32];
    std::strftime(str, 32,"--%m-%d", std::localtime(&value));

    return std::string{str};
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::gMonthDay
 */
using GMonthDay = registry::LiteralDatatypeImpl<registry::xsd_gMonthDay>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_GMONTHDAY_HPP
