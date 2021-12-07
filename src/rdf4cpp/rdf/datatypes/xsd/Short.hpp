//
// Created by kaimal on 26.11.21.
//

/**
 * @file Registers xsd:short with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_SHORT_HPP
#define RDF4CPP_XSD_SHORT_HPP

#include <cstdint>
#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Short = int16_t;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#short">xsd:short</a>
}


namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::Short>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#short"; }

template<>
inline xsd::Short RegisteredDatatype<xsd::Short>::from_string(const std::string &s) {
    auto int16_val = std::stoi(s);
    if(int16_val < -32768 || int16_val > 32767) throw std::runtime_error("XSD Parsing Error");
    return int16_val;
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_SHORT_HPP