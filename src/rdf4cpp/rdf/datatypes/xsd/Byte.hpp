//
// Created by kaimal on 26.11.21.
//

/**
 * @file Registers xsd:byte with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_BYTE_HPP
#define RDF4CPP_XSD_BYTE_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Byte = int8_t;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#byte">xsd:byte</a>
}


namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::Byte>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#byte"; }

template<>
inline xsd::Byte RegisteredDatatype<xsd::Byte>::from_string(const std::string &s) {
    auto int8_val = std::stoi(s);
    if(int8_val < -128 || int8_val > 127) throw "Invalid";
    return int8_val;
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_BYTE_HPP