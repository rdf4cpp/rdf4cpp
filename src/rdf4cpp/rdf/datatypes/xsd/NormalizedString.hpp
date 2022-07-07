
/**
 * @file Registers xsd:normalizedString with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_NORMALIZEDSTRING_HPP
#define RDF4CPP_XSD_NORMALIZEDSTRING_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_normalizedString{"http://www.w3.org/2001/XMLSchema#normalizedString"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_normalizedString> {
    using cpp_datatype = std::string;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_normalizedString>::cpp_type LiteralDatatypeImpl<xsd_normalizedString>::from_string(std::string_view s) {
    const std::regex non_ns_regex("(\\n)|(\\t)|(\\r)");
    if (std::regex_search(s.data(), non_ns_regex)) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return s.data();
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::normalizedString
 */
using NormalizedString = registry::LiteralDatatypeImpl<registry::xsd_normalizedString>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NORMALIZEDSTRING_HPP
