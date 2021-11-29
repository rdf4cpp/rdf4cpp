//
// Created by kaimal on 29.11.21.
//

/**
 * @file Registers xsd:unsignedByte with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_UNSIGNEDBYTE_HPP
#define RDF4CPP_XSD_UNSIGNEDBYTE_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>
        namespace rdf4cpp::rdf::datatypes::xsd {
    using UnsignedByte = uint8_t;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#unsignedByte">xsd:unsignedByte</a>
}

namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::UnsignedByte>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#unsignedByte"; }

template<>
inline xsd::UnsignedByte RegisteredDatatype<xsd::UnsignedByte>::from_string(const std::string &s) {
    auto int8_val = std::stoi(s);
    if(int8_val < 0 || int8_val > 255) throw "Invalid";
    return int8_val;
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_UNSIGNEDBYTE_HPP
