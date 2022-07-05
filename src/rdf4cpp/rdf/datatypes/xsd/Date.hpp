
/**
 * @file Registers xsd:date with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_DATE_HPP
#define RDF4CPP_XSD_DATE_HPP

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
constexpr static registry::ConstexprString xsd_date{"http://www.w3.org/2001/XMLSchema#date"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_date> {
    using cpp_datatype = time_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_date>::cpp_type LiteralDatatypeImpl<xsd_date>::from_string(std::string_view s) {

    const std::regex date_regex(R"(-?([1-9][0-9]{3,}|0[0-9]{3})-(0[1-9]|1[0-2])-(0[1-9]|[12][0-9]|3[01])(Z|(\+|-)((0[0-9]|1[0-3]):[0-5][0-9]|14:00))?)");

    std::map<std::string , std::string> time_zones = {
            { "+14:00", "LINT" },
            { "+13:00", "TOT" },
            { "+12:45", "CHAST" },
            { "+12:00" , "ANAT"},
            { "+11:00" , "SBT"},
            { "+10:30" , "LHST"},
            { "+10:00" , "AEST"},
            { "+09:30" , "ACST"},
            { "+09:00" , "JST"},
            { "+08:45" , "ACWST"},
            { "+08:00" , "CST"},
            { "+07:00" , "WIB"},
            { "+06:30" , "MMT"},
            { "+06:00" , "BST"},
            { "+05:45" , "NPT"},
            { "+05:30" , "IST"},
            { "+05:00" , "UZT"},
            { "+04:30" , "IRDT"},
            { "+04:00" , "GST"},
            { "+03:00" , "MSK"},
            { "+02:00" , "CEST"},
            { "+01:00" , "BST"},
            { "+00:00" , "GMT"},
            { "-01:00" , "CVT"},
            { "-02:30" , "WGST"},
            { "-02:30" , "NDT"},
            { "-03:00" , "ART"},
            { "-04:00" , "EDT"},
            { "-05:00" , "CDT"},
            { "-06:00" , "CST"},
            { "-07:00" , "PDT"},
            { "-08:00" , "AKDT"},
            { "-09:00" , "HDT"},
            { "-09:30" , "MART"},
            { "-10:00" , "HST"},
            { "-11:00" , "NUT"},
            { "-12:00" , "AoE"}
    };

    if (std::regex_match(s.data(), date_regex)) {

        std::string str = s.data(), date;

        if(str.find('Z') != std::string::npos || str.find('+') != std::string::npos || str.find('-') != std::string::npos) {
            date = str.substr(0, 10); //Retrieve only date value

            std::string key;
            if(str.find('Z') != std::string::npos) {
                key = "+00.00";
            }else{
                key = str.substr(10, str.length() - 1);
            }

            std::string tz = time_zones[key] + key;
            setenv("TZ", tz.c_str(), 1); //sets environment timezone based on key
            tzset(); //sets time zone

        }

        struct tm tm {};
        strptime(date.c_str(), "%Y-%m-%d", &tm);

        //default values
        tm.tm_sec = 0;     // seconds of minutes from 0 to 61
        tm.tm_min = 0;     // minutes of hour from 0 to 59
        tm.tm_hour = 0;    // hours of day from 0 to 24
        tm.tm_isdst = -1;  // daylight saving set to negative since no data available

        return mktime(&tm);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_date>::to_string(const cpp_type &value) {

    struct  tm *tm = std::localtime(&value);

    char str[32];
    std::string date_str;

    if(tm->tm_gmtoff == 0){
        std::strftime(str, 32, "%Y-%m-%dZ", std::localtime(&value));
        date_str = str;
    }else {
        std::strftime(str, 32, "%Y-%m-%d", std::localtime(&value));
        char sign = tm->tm_gmtoff < 0 ? '-' : '+';
        date_str = str;
        date_str.push_back(sign);

        //Converting tm_gmtoff in Seconds to Hours and Minutes
        int hours = (tm->tm_gmtoff / 60) / 60;
        int minutes = (tm->tm_gmtoff / 60) % 60;

        //Formatting Hours and Minutes
        std::stringstream ss;
        ss << std::setw(2) << std::setfill('0') << hours;

        //Appending Hours of time zone to date
        date_str = date_str.append(ss.str());

        //Pushing hour minutes separator
        date_str.push_back(':');

        //Clears stream for converting minute
        ss.str("");
        ss << std::setw(2) << std::setfill('0') << minutes;

        //Appending Minutes of time zone to date
        date_str = date_str.append(ss.str());
    }
    return date_str;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::date
 */
using Date = registry::LiteralDatatypeImpl<registry::xsd_date>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DATE_HPP
