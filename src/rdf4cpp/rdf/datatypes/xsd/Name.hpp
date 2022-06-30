
/**
 * @file Registers xsd:Name with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_NAME_HPP
#define RDF4CPP_XSD_NAME_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_name{"http://www.w3.org/2001/XMLSchema#name"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_name> {
    using cpp_datatype = std::string;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_name>::cpp_type LiteralDatatypeImpl<xsd_name>::from_string(std::string_view s) {
    const std::regex non_name_regex("(\\n)|(\\t)|(\\r)|([ ]{2,})|(^[ ].*)|(.*[ ]$)");
    if (std::regex_search(s.data(), non_name_regex)) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return s.data();
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::Name
 */
using Name = registry::LiteralDatatypeImpl<registry::xsd_name>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NAME_HPP
