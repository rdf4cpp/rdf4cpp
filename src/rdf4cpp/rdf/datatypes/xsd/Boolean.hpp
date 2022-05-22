
/**
* @file Registers xsd:boolean with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_BOOLEAN_HPP
#define RDF4CPP_XSD_BOOLEAN_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Boolean = bool;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#boolean">xsd:boolean</a>
}

namespace rdf4cpp::rdf::datatypes {
constexpr const char xsd_boolean[] = "http://www.w3.org/2001/XMLSchema#boolean";

template<>
inline std::string RegisteredDatatype<xsd::Boolean, xsd_boolean>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#boolean"; }

template<>
inline xsd::Boolean RegisteredDatatype<xsd::Boolean, xsd_boolean>::from_string(std::string_view s) {
    if (s == "true" || s == "1") return true;
    else if (s == "false" || s == "0")
        return false;
    else
        throw std::runtime_error("XSD Parsing Error");
}
}  // namespace rdf4cpp::rdf::datatypes
#endif  //RDF4CPP_XSD_BOOLEAN_HPP
