
/**
 * @file Registers xsd:gYear with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_GYEAR_HPP
#define RDF4CPP_XSD_GYEAR_HPP

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
constexpr static registry::ConstexprString xsd_gYear{"http://www.w3.org/2001/XMLSchema#gYear"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_gYear> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_gYear>::cpp_type LiteralDatatypeImpl<xsd_gYear>::from_string(std::string_view s) {

    const std::regex gYear_regex("-?([1-9][0-9]{3,}|0[0-9]{3})(Z|(\\+|-)((0[0-9]|1[0-3]):[0-5][0-9]|14:00))?");

    if (std::regex_match(s.data(), gYear_regex)) {

        struct tm tm{};
        strptime(s.data(), "-%Y", &tm);

        //default values
        tm.tm_sec = 0; // seconds of minutes from 0 to 61
        tm.tm_min = 0; // minutes of hour from 0 to 59
        tm.tm_hour = 0; // hours of day from 0 to 24
        tm.tm_mon = 0; // month of year from 0 to 11
        tm.tm_mday = 1; // date of the month
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
inline std::string LiteralDatatypeImpl<xsd_gYear>::to_string(const cpp_type &value) {

    char str[32];
    std::strftime(str, 32,"-%Y", std::localtime(&value));

    return std::string{str};
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::gYear
 */
using GYear = registry::LiteralDatatypeImpl<registry::xsd_gYear>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_GYEAR_HPP
