
/**
 * @file Registers xsd:short with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_SHORT_HPP
#define RDF4CPP_XSD_SHORT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_short{"http://www.w3.org/2001/XMLSchema#short"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_short> {
    using cpp_datatype = int16_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_short>::cpp_type LiteralDatatypeImpl<xsd_short>::from_string(std::string_view s) {

    const std::regex long_regex("[\\-+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto int16_val = std::strtol(s.data(), nullptr, 10);
        if (int16_val < -32768 || int16_val > 32767) throw std::runtime_error("XSD Parsing Error");
        return int16_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::short
 */
using Short = registry::LiteralDatatypeImpl<registry::xsd_short>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_SHORT_HPP