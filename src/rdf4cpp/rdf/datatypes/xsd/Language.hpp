
/**
 * @file Registers xsd:language with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_LANGUAGE_HPP
#define RDF4CPP_XSD_LANGUAGE_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_language{"http://www.w3.org/2001/XMLSchema#language"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_language> {
    using cpp_datatype = std::string;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_language>::cpp_type LiteralDatatypeImpl<xsd_language>::from_string(std::string_view s) {
    const std::regex language_regex("[a-zA-Z]{1,8}(-[a-zA-Z0-9]{1,8})*");
    if (std::regex_match(s.data(), language_regex)) {
        return s.data();
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::language
 */
using Language = registry::LiteralDatatypeImpl<registry::xsd_language>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_LANGUAGE_HPP
