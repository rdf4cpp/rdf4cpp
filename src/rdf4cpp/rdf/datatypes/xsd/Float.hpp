/**
 * @file Registers xsd:float with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_FLOAT_HPP
#define RDF4CPP_XSD_FLOAT_HPP


#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {
using Float = float;  //!< Implements <a href="http://www.w3.org/2001/XMLSchema#float">xsd:float</a>
}


namespace rdf4cpp::rdf::datatypes {
template<>
inline std::string RegisteredDatatype<xsd::Float>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#float"; }
template<>
inline float RegisteredDatatype<xsd::Float>::from_string(const std::string &s) {
    return std::stof(s);
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_FLOAT_HPP
