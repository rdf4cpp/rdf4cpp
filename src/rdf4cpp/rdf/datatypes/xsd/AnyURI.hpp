
/**
 * @file Registers xsd:anyURI with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_ANYURI_HPP
#define RDF4CPP_XSD_ANYURI_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_anyURI{"http://www.w3.org/2001/XMLSchema#anyURI"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_anyURI> {
    using cpp_datatype = std::string;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_anyURI>::cpp_type LiteralDatatypeImpl<xsd_anyURI>::from_string(std::string_view s) {
    return s.data();
}
}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::anyURI
 */
using AnyURI = registry::LiteralDatatypeImpl<registry::xsd_anyURI>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_ANYURI_HPP
