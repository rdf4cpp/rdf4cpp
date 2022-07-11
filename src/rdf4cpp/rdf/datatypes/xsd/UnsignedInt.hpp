
/**
 * @file Registers xsd:unsignedInt with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_UNSIGNEDINT_HPP
#define RDF4CPP_XSD_UNSIGNEDINT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_unsignedInt{"http://www.w3.org/2001/XMLSchema#unsignedInt"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_unsignedInt> {
    using cpp_datatype = uint32_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_unsignedInt>::cpp_type LiteralDatatypeImpl<xsd_unsignedInt>::from_string(std::string_view s) {

    const std::regex long_regex("[\\+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto uint32_val = std::strtol(s.data(), nullptr, 10);
        if (uint32_val < std::numeric_limits<uint32_t>::min() || uint32_val > std::numeric_limits<uint32_t>::max()) throw std::runtime_error("XSD Parsing Error");
        return uint32_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::unsignedInt
 */
using UnsignedInt = registry::LiteralDatatypeImpl<registry::xsd_unsignedInt>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_UNSIGNEDINT_HPP
