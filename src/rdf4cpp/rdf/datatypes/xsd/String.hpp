
/**
 * @file Registers xsd:string with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_STRING_HPP
#define RDF4CPP_XSD_STRING_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_string{"http://www.w3.org/2001/XMLSchema#string"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_string> {
    using cpp_datatype = std::string;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_string>::cpp_type LiteralDatatypeImpl<xsd_string>::from_string(std::string_view s) {
    return std::string{ s };
}
}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::string
 */
using String = registry::LiteralDatatypeImpl<registry::xsd_string>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_STRING_HPP
