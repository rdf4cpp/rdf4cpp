
/**
* @file Registers xsd:boolean with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_BOOLEAN_HPP
#define RDF4CPP_XSD_BOOLEAN_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_boolean{"http://www.w3.org/2001/XMLSchema#boolean"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_boolean> {
    using cpp_datatype = bool;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_boolean>::cpp_type LiteralDatatypeImpl<xsd_boolean>::from_string(std::string_view s) {
    if (s == "true" || s == "1") return true;
    else if (s == "false" || s == "0")
        return false;
    else
        throw std::runtime_error("XSD Parsing Error");
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::boolean
 */
using Boolean = registry::LiteralDatatypeImpl<registry::xsd_boolean>;
}  // namespace rdf4cpp::rdf::datatypes::xsd


#endif  //RDF4CPP_XSD_BOOLEAN_HPP
