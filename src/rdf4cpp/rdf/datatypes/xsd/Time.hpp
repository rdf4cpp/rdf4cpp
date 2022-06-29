
/**
 * @file Registers xsd:time with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_TIME_HPP
#define RDF4CPP_XSD_TIME_HPP

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
constexpr static registry::ConstexprString xsd_time{"http://www.w3.org/2001/XMLSchema#time"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_time> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_time>::cpp_type LiteralDatatypeImpl<xsd_time>::from_string(std::string_view s) {

    const std::regex time_regex(R"((([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9](\.[0-9]+)?|(24:00:00(\.0+)?))(Z|(\+|-)((0[0-9]|1[0-3]):[0-5][0-9]|14:00))?)");

    if (std::regex_match(s.data(), time_regex)) {
        struct tm tm {};
        strptime(s.data(), "%H:%M:%S", &tm);

        //default values
        tm.tm_year = 1900;  // year since 1900
        tm.tm_mon = 0;      // month of year from 0 to 11
        tm.tm_mday = 1;     //date of the month
        tm.tm_isdst = -1;   // value should be set even if not used

        return mktime(&tm);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_time>::to_string(const cpp_type &value) {

    char str[32];
    std::strftime(str, 32, "%H:%M:%S", std::localtime(&value));

    return std::string{str};
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::time
 */
using Time = registry::LiteralDatatypeImpl<registry::xsd_time>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_TIME_HPP