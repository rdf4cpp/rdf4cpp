
/**
 * @file Registers xsd:dayTimeDuration with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_DAYTIMEDURATION_HPP
#define RDF4CPP_XSD_DAYTIMEDURATION_HPP

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
constexpr static registry::ConstexprString xsd_dtDuration{"http://www.w3.org/2001/XMLSchema#dayTimeDuration"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_dtDuration> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_dtDuration>::cpp_type LiteralDatatypeImpl<xsd_dtDuration>::from_string(std::string_view s) {

    const std::regex dt_duration_regex("-?P((([0-9]+D)"
                                    "(T(([0-9]+H)([0-9]+M)?([0-9]+(\\.[0-9]+)?S)?"
                                    "|([0-9]+M)([0-9]+(\\.[0-9]+)?S)?"
                                    "|([0-9]+(\\.[0-9]+)?S)))?)"
                                    "|(T(([0-9]+H)([0-9]+M)?([0-9]+(\\.[0-9]+)?S)?"
                                    "|([0-9]+M)([0-9]+(\\.[0-9]+)?S)?"
                                    "|([0-9]+(\\.[0-9]+)?S))))");

    if (std::regex_match(s.data(), dt_duration_regex)) {

        struct tm tm {};

        //default values
        tm.tm_year = 0;    // year since 1900
        tm.tm_mon = 0;     // month of year from 0 to 11
        tm.tm_mday = 0;    // day
        tm.tm_sec = 0;     // seconds of minutes from 0 to 61
        tm.tm_min = 0;     // minutes of hour from 0 to 59
        tm.tm_hour = 0;    // hours of day from 0 to 24
        tm.tm_isdst = -1;  // daylight saving

        std::string str = s.data();

        ulong start = 0, end = 0;
        start = str.find('P');
        end = str.find('D');
        if (end != std::string::npos) {
            start++;
            tm.tm_mday = std::stoi(str.substr(start, end - start));
        }
        end = str.find('T');
        start = end;
        if (end != std::string::npos) {

            end = str.find('H');
            if (end != std::string::npos) {
                start++;
                tm.tm_hour = std::stoi(str.substr(start, end - start));
                start = end;
            }
            end = str.find('M', start);
            if (end != std::string::npos) {
                start++;
                tm.tm_min = std::stoi(str.substr(start, end - start));
                start = end;
            }
            end = str.find('S');
            if (end != std::string::npos) {
                start++;
                tm.tm_sec = std::stoi(str.substr(start, end - start));
            }
        }

        return mktime(&tm);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Formats DayTimeDuration based on rdf specification
 */
inline void fmt_dtduration(std::string *duration, int value, char tag) {

    if (tag == 'T') duration->push_back(tag);
    if (value != 0) {
        switch (tag) {
            case 'H':
            case 'M':
            case 'S':
                if (duration->find('T') == std::string::npos) duration->push_back('T');
                break;
            default:
                break;
        }
        duration->append(std::to_string(value));  //Adds value to the duration
        duration->push_back(tag);                 //Adds identifier of the value
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_dtDuration>::to_string(const cpp_type &value) {

    struct tm *tm = localtime(&value);

    std::string duration{"P"};

    if(tm->tm_year >= 0) {
        if(tm->tm_year > 0) {
            fmt_dtduration(&duration, (tm->tm_year * 365) + (tm->tm_yday + 1), 'D');
        } else if(tm->tm_mon > 0)  {
            fmt_dtduration(&duration, tm->tm_yday + 1, 'D');
        } else{
            fmt_dtduration(&duration, tm->tm_mday, 'D');
        }
    }
    fmt_dtduration(&duration, tm->tm_hour, 'H');
    fmt_dtduration(&duration, tm->tm_min, 'M');
    fmt_dtduration(&duration, tm->tm_sec, 'S');

    return duration;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::dayTimeDuration
 */
using DayTimeDuration = registry::LiteralDatatypeImpl<registry::xsd_dtDuration>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DAYTIMEDURATION_HPP
