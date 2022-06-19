
/**
 * @file Registers xsd:negativeInteger with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_NEGATIVEINTEGER_HPP
#define RDF4CPP_XSD_NEGATIVEINTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_negativeInteger{"http://www.w3.org/2001/XMLSchema#negativeInteger"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_negativeInteger> {
    using cpp_datatype = int64_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_negativeInteger>::cpp_type LiteralDatatypeImpl<xsd_negativeInteger>::from_string(std::string_view s) {

    const std::regex negativeInteger_regex("[\\-][0-9]+");

    if (std::regex_match(s.data(), negativeInteger_regex)) {
        cpp_type val = std::strtol(s.data(), nullptr, 10);

        if (val < 0) {
            return val;
        } else {
            throw std::runtime_error("XSD Parsing Error");
        }
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::negativeInteger
 */
using NegativeInteger = registry::LiteralDatatypeImpl<registry::xsd_negativeInteger>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NEGATIVEINTEGER_HPP
