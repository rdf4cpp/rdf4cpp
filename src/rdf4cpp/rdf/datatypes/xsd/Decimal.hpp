
/**
* @file Registers xsd:float with DatatypeRegistry
*/

#ifndef RDF4CPP_DECIMAL_HPP
#define RDF4CPP_DECIMAL_HPP

#include <cstdint>
#include <ostream>
#include <regex>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Decimal = double;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#decimal">xsd:decimal</a>
}

namespace rdf4cpp::rdf::datatypes {
constexpr const char xsd_decimal[] = "http://www.w3.org/2001/XMLSchema#decimal";

template<>
inline std::string RegisteredDatatype<xsd::Decimal, xsd_decimal>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#decimal"; }

template<>
inline xsd::Decimal RegisteredDatatype<xsd::Decimal, xsd_decimal>::from_string(std::string_view s) {

    const std::regex decimal_regex("(\\+|-)?([0-9]+(\\.[0-9]*)?|\\.[0-9]+)");

    if (std::regex_match(s.data(), decimal_regex)) {
        return std::strtod(s.data(), nullptr);
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
template<>
inline std::string RegisteredDatatype<xsd::Decimal, xsd_decimal>::to_string(const xsd::Decimal &value) {

    std::ostringstream str_os;
    // Set Fixed -Point Notation
    str_os << std::fixed;
    str_os << value;
    // Get string from output string stream
    std::string str = str_os.str();
    return str;
}
}  // namespace rdf4cpp::rdf::datatypes
#endif  //RDF4CPP_DECIMAL_HPP
