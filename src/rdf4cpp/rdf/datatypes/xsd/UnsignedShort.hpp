
/**
 * @file Registers xsd:unsignedShort with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_UNSIGNEDSHORT_HPP
#define RDF4CPP_XSD_UNSIGNEDSHORT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_unsignedShort{"http://www.w3.org/2001/XMLSchema#unsignedShort"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_unsignedShort> {
    using cpp_datatype = u_int16_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_unsignedShort>::cpp_type LiteralDatatypeImpl<xsd_unsignedShort>::from_string(std::string_view s) {

    const std::regex long_regex("[\\+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto uint16_val = std::strtol(s.data(), nullptr, 10);
        if (uint16_val < 0 || uint16_val > 65535) throw std::runtime_error("XSD Parsing Error");
        return uint16_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::unsignedShort
 */
using UnsignedShort = registry::LiteralDatatypeImpl<registry::xsd_unsignedShort>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_UNSIGNEDSHORT_HPP
