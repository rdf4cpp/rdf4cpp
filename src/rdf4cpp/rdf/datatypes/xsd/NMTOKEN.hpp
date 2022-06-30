
/**
 * @file Registers xsd:NMTOKEN with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_NMTOKEN_HPP
#define RDF4CPP_XSD_NMTOKEN_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_NMTOKEN{"http://www.w3.org/2001/XMLSchema#NMTOKEN"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_NMTOKEN> {
    using cpp_datatype = std::string;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_NMTOKEN>::cpp_type LiteralDatatypeImpl<xsd_NMTOKEN>::from_string(std::string_view s) {
    const std::regex non_nmToken_regex("(\\n)|(\\t)|(\\r)|([ ]{2,})|(^[ ].*)|(.*[ ]$)");
    if (std::regex_search(s.data(), non_nmToken_regex)) {
        throw std::runtime_error("XSD Parsing Error");
    } else {
        return s.data();
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry
namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::NMTOKEN
 */
using NMTOKEN = registry::LiteralDatatypeImpl<registry::xsd_NMTOKEN>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NMTOKEN_HPP
