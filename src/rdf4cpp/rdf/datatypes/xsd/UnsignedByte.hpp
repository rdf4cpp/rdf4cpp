
/**
 * @file Registers xsd:unsignedByte with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_UNSIGNEDBYTE_HPP
#define RDF4CPP_XSD_UNSIGNEDBYTE_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_unsignedByte{"http://www.w3.org/2001/XMLSchema#unsignedByte"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_unsignedByte> {
    using cpp_datatype = uint8_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_unsignedByte>::cpp_type LiteralDatatypeImpl<xsd_unsignedByte>::from_string(std::string_view s) {

    const std::regex long_regex("[\\+]?[0-9]+");

    if (std::regex_match(s.data(), long_regex)) {
        auto uint8_val = atoi(s.data());
        if (uint8_val < 0 || uint8_val > 255) throw std::runtime_error("XSD Parsing Error");
        return uint8_val;
    } else {
        throw std::runtime_error("XSD Parsing Error");
    }
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_unsignedByte>::to_string(const cpp_type &value) {

    /**
     * If not converted to uint16_t, conversion to string generates a byte representation '\x01' which doest match the lexical representation.
     */
    int conv_val = uint16_t(value);
    std::stringstream str_s;
    str_s << conv_val;
    std::string str = str_s.str();
    return str;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::unsignedByte
 */
using UnsignedByte = registry::LiteralDatatypeImpl<registry::xsd_unsignedByte>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_UNSIGNEDBYTE_HPP
