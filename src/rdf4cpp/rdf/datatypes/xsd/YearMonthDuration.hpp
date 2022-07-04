
/**
 * @file Registers xsd:yearMonthDuration with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_YEARMONTHDURATION_HPP
#define RDF4CPP_XSD_YEARMONTHDURATION_HPP

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
constexpr static registry::ConstexprString xsd_ymDuration{"http://www.w3.org/2001/XMLSchema#yearMonthDuration"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_ymDuration> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_ymDuration>::cpp_type LiteralDatatypeImpl<xsd_ymDuration>::from_string(std::string_view s) {

    const std::regex ym_duration_regex("-?P((([0-9]+Y)([0-9]+M)?)|([0-9]+M))");

    if (std::regex_match(s.data(), ym_duration_regex)) {

        struct tm tm {};

        //By default these values should be set as following for proper calculation of value.
        tm.tm_year = 0;    // year since 1900
        tm.tm_mon = 0;     // month of year from 0 to 11
        tm.tm_mday = 1;    // day
        tm.tm_sec = 0;     // seconds of minutes from 0 to 61
        tm.tm_min = 0;     // minutes of hour from 0 to 59
        tm.tm_hour = 0;    // hours of day from 0 to 24
        tm.tm_isdst = -1;  // daylight saving

        std::string str = s.data();

        ulong start = 0, end = 0;
        start = str.find('P');
        end = str.find('Y');
        if (end != std::string::npos) {
            start++;
            tm.tm_year = std::stoi(str.substr(start, end - start));
            start = end;
        }
        end = str.find('M');
        if (end != std::string::npos) {
            start++;
            tm.tm_mon = std::stoi(str.substr(start, end - start));
            start = end;
        }

        return mktime(&tm);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Formats Duration based of rdf specification
 */
inline void fmt_ymduration(std::string *duration, int value, char tag) {

    if (value != 0) {
        duration->append(std::to_string(value));  //Adds value to the duration
        duration->push_back(tag);                 //Adds identifier of the value
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_ymDuration>::to_string(const cpp_type &value) {

    struct tm *tm = localtime(&value);

    std::string duration{"P"};

    fmt_ymduration(&duration, tm->tm_year, 'Y');
    fmt_ymduration(&duration, tm->tm_mon, 'M');

    return duration;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::yearMonthDuration
 */
using YearMonthDuration = registry::LiteralDatatypeImpl<registry::xsd_ymDuration>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_YEARMONTHDURATION_HPP
