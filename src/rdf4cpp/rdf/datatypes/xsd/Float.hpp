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
constexpr const char xsd_float[] = "http://www.w3.org/2001/XMLSchema#float";

template<>
inline std::string RegisteredDatatype<xsd::Float, xsd_float>::datatype_iri() noexcept { return "http://www.w3.org/2001/XMLSchema#float"; }
template<>
inline float RegisteredDatatype<xsd::Float, xsd_float>::from_string(std::string_view s) {
    return std::stof(std::string{s.data()});
}
}  // namespace rdf4cpp::rdf::datatypes

#endif  //RDF4CPP_XSD_FLOAT_HPP
