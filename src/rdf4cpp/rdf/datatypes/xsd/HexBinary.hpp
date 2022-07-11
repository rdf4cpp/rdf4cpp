
/**
 * @file Registers xsd:hexBinary with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_HEXBINARY_HPP
#define RDF4CPP_XSD_HEXBINARY_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_hexBinary{"http://www.w3.org/2001/XMLSchema#hexBinary"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_hexBinary> {
    using cpp_datatype = std::vector<std::int16_t>;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_hexBinary>::cpp_type LiteralDatatypeImpl<xsd_hexBinary>::from_string(std::string_view s) {

    const std::regex hexBinary_regex("([0-9a-fA-F]{2})*");
    int16_t hexOctet;
    std::vector<int16_t> hexBinary_val;
    std::string str = s.data();
    if (std::regex_match(str, hexBinary_regex)) {
        for(std::string::size_type pos = 0; pos < str.size(); pos+=4) {
            std::stringstream iss(str.substr(pos, 4));
            iss >> std::hex >> hexOctet;
            hexBinary_val.push_back(hexOctet);
        }
    }else {
        throw std::runtime_error("XSD Parsing Error");
    }
    return hexBinary_val;
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_hexBinary>::to_string(const cpp_type &value) {

    std::string res;
    for(auto it = std::begin(value); it != std::end(value); ++it) {
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(4) << std::hex << *it;
        res.append(ss.str());
    }return res;
}
}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::hexBinary
 */
using HexBinary = registry::LiteralDatatypeImpl<registry::xsd_hexBinary>;
}  // namespace rdf4cpp::rdf::datatypes::xsd
#endif  //RDF4CPP_XSD_HEXBINARY_HPP
