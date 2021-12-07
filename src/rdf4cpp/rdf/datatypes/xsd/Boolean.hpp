//
// Created by kaimal on 02.12.21.
//

#ifndef RDF4CPP_XSD_BOOLEAN_HPP
#define RDF4CPP_XSD_BOOLEAN_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Boolean = bool;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#boolean">xsd:boolean</a>
}

namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::Boolean>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#boolean"; }

template<>
inline xsd::Boolean RegisteredDatatype<xsd::Boolean>::from_string(const std::string &s) {
    if(s == "true" || s == "1") return true;
    else if(s == "false" || s == "0") return false;
    else throw std::runtime_error("XSD Parsing Error");
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_BOOLEAN_HPP
