
/**
 * @file Registers xsd:byte with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_BYTE_HPP
#define RDF4CPP_XSD_BYTE_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_byte{"http://www.w3.org/2001/XMLSchema#byte"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_byte> {
    using cpp_datatype = int8_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_byte>::cpp_type LiteralDatatypeImpl<xsd_byte>::from_string(std::string_view s) {

    const std::regex long_regex("[\\-+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto int8_val = (atoi(s.data()));
        if (int8_val < -128 || int8_val > 127) throw std::runtime_error("XSD Parsing Error");
        return int8_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_byte>::to_string(const cpp_type &value) {

    /**
     * If not converted to int16_t, conversion to string generates a byte representation '\x01' which doest match the lexical representation.
     */
    int conv_val = int16_t(value);
    std::stringstream str_s;
    str_s << conv_val;
    std::string str = str_s.str();
    return str;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::byte
 */
using Byte = registry::LiteralDatatypeImpl<registry::xsd_byte>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_BYTE_HPP