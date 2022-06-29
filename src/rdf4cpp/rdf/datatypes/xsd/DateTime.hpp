
/**
 * @file Registers xsd:dateTime with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_DATETIME_H
#define RDF4CPP_XSD_DATETIME_H

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
constexpr static registry::ConstexprString xsd_dateTime{"http://www.w3.org/2001/XMLSchema#dateTime"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_dateTime> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_dateTime>::cpp_type LiteralDatatypeImpl<xsd_dateTime>::from_string(std::string_view s) {

    const std::regex dateTime_regex("(-?([1-9][0-9]{3,}|0[0-9]{3})-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])"
                                    "T(([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9](\\.[0-9]+)?|(24:00:00(\\.0+)?))(Z|(\\+|-)((0[0-9]|1[0-3]):[0-5][0-9]|14:00))?)");
    if (std::regex_match(s.data(), dateTime_regex)) {
        struct tm tm{};
        strptime(s.data(), "%Y-%m-%dT%H:%M:%S", &tm);

        //default values
        tm.tm_isdst = -1; // daylight saving

        return mktime(&tm);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_dateTime>::to_string(const cpp_type &value) {

    char str[32];
    std::strftime(str, 32, "%Y-%m-%dT%H:%M:%S", std::localtime(&value));

    return std::string{str};
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::dateTime
 */
using DateTime = registry::LiteralDatatypeImpl<registry::xsd_dateTime>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DATETIME_H
