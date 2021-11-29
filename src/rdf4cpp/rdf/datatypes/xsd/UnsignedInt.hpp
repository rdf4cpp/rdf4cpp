//
// Created by kaimal on 29.11.21.
//

/**
 * @file Registers xsd:unsignedInt with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_UNSIGNEDSHORT_HPP
#define RDF4CPP_XSD_UNSIGNEDSHORT_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using UnsignedInt = uint32_t;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#unsignedInt">xsd:unsignedInt</a>
}

namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::UnsignedInt>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#unsignedInt"; }

template<>
inline xsd::UnsignedInt RegisteredDatatype<xsd::UnsignedInt>::from_string(const std::string &s) {
    auto int32_val = std::stoi(s);
    if(int32_val < 0 || int32_val > 4294967295) throw "Invalid";
    return int32_val ;
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_UNSIGNEDSHORT_HPP

