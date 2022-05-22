
/**
 * @file Registers xsd:string with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_STRING_HPP
#define RDF4CPP_XSD_STRING_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using String = std::string;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#string">xsd:string</a>
}

namespace rdf4cpp::rdf::datatypes {
constexpr const char xsd_string[] = "http://www.w3.org/2001/XMLSchema#string";

template<>
inline std::string RegisteredDatatype<xsd::String, xsd_string>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#string"; }

template<>
inline xsd::String RegisteredDatatype<xsd::String, xsd_string>::from_string(std::string_view s) {
    return s.data();
}
}  // namespace rdf4cpp::rdf::datatypes
#endif  //RDF4CPP_XSD_STRING_HPP
