
/**
 * @file Registers xsd:unsignedLong with DatatypeRegistry
 */

#ifndef RDF4CPP_UNSIGNEDLONG_HPP
#define RDF4CPP_UNSIGNEDLONG_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_unsignedLong{"http://www.w3.org/2001/XMLSchema#unsignedLong"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_unsignedLong> {
    using cpp_datatype = uint64_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_unsignedLong>::cpp_type LiteralDatatypeImpl<xsd_unsignedLong>::from_string(std::string_view s) {

    const std::regex long_regex("[\\+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto ulong_val = std::strtoul(s.data(), nullptr, 10);
        if (ulong_val < std::numeric_limits<uint64_t>::min() || ulong_val > std::numeric_limits<uint64_t>::max()) throw std::runtime_error("XSD Parsing Error");
        return ulong_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::unsignedLong
 */
using UnsignedLong = registry::LiteralDatatypeImpl<registry::xsd_unsignedLong>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_UNSIGNEDLONG_HPP
