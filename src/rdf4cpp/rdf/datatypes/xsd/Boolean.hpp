
/**
* @file Registers xsd:boolean with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_BOOLEAN_HPP
#define RDF4CPP_XSD_BOOLEAN_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <sstream>
#include <stdexcept>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static util::ConstexprString xsd_boolean{"http://www.w3.org/2001/XMLSchema#boolean"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_boolean> {
    using cpp_datatype = bool;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<xsd_boolean>::cpp_type capabilities::Default<xsd_boolean>::from_string(std::string_view s) {
    if (s == "true" || s == "1") return true;
    else if (s == "false" || s == "0")
        return false;
    else
        throw std::runtime_error("XSD Parsing Error");
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string capabilities::Default<xsd_boolean>::to_string(const cpp_type &value) {

    std::ostringstream str_os;
    str_os << std::boolalpha << value;
    std::string str = str_os.str();
    return str;
}

template<>
inline bool capabilities::Logical<xsd_boolean>::effective_boolean_value(cpp_type const &value) noexcept {
    return value;
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

/**
 * Implementation of xsd::boolean
 */
struct Boolean : registry::LiteralDatatypeImpl<registry::xsd_boolean,
                                               registry::capabilities::Logical> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd


#endif  //RDF4CPP_XSD_BOOLEAN_HPP
