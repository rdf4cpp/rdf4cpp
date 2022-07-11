
/**
 * @file Registers xsd:nonPositiveInteger with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_NONPOSITIVEINTEGER_HPP
#define RDF4CPP_XSD_NONPOSITIVEINTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_nonPositiveInteger{"http://www.w3.org/2001/XMLSchema#nonPositiveInteger"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_nonPositiveInteger> {
    using cpp_datatype = int64_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_nonPositiveInteger>::cpp_type LiteralDatatypeImpl<xsd_nonPositiveInteger>::from_string(std::string_view s) {

    const std::regex npInteger_regex("[\\-]?[0-9]+");

    if (std::regex_match(s.data(), npInteger_regex)) {
        cpp_type val = std::strtol(s.data(), nullptr, 10);

        if (val <= 0) {
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
 * Implementation of xsd::nonPositiveInteger
 */
using NonPositiveInteger = registry::LiteralDatatypeImpl<registry::xsd_nonPositiveInteger>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NONPOSITIVEINTEGER_HPP
