
/**
 * @file Registers xsd:long with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_LONG_HPP
#define RDF4CPP_XSD_LONG_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_long{"http://www.w3.org/2001/XMLSchema#long"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_long> {
    using cpp_datatype = int64_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_long>::cpp_type LiteralDatatypeImpl<xsd_long>::from_string(std::string_view s) {

    const std::regex long_regex("[\\-+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto long_val = std::strtol(s.data(), nullptr, 10);
        if (long_val < -9223372036854775808 || long_val > 9223372036854775807) throw std::runtime_error("XSD Parsing Error");
        return long_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }

}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::long
 */
using Long = registry::LiteralDatatypeImpl<registry::xsd_long>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_LONG_HPP
