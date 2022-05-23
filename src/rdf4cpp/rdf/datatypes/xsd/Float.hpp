/**
 * @file Registers xsd:float with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_FLOAT_HPP
#define RDF4CPP_XSD_FLOAT_HPP


#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <cstdint>
#include <ostream>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_float{"http://www.w3.org/2001/XMLSchema#float"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_float> {
    using cpp_datatype = float;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_float>::cpp_type LiteralDatatypeImpl<xsd_float>::from_string(std::string_view s) {
    return std::stof(std::string{s.data()});
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string LiteralDatatypeImpl<xsd_float>::to_string(const cpp_type &value) {

    std::ostringstream str_os;
    // Set Fixed -Point Notation
    str_os << std::fixed;
    str_os << value;
    // Get string from output string stream
    std::string str = str_os.str();
    return str;
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::float
 */
using Decimal = registry::LiteralDatatypeImpl<registry::xsd_decimal>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_FLOAT_HPP
