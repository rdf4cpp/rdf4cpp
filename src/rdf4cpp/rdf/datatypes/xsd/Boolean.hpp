
/**
* @file Registers xsd:boolean with DatatypeRegistry
*/

#ifndef RDF4CPP_XSD_BOOLEAN_HPP
#define RDF4CPP_XSD_BOOLEAN_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <sstream>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {
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
                                               registry::capabilities::Logical,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd


#endif  //RDF4CPP_XSD_BOOLEAN_HPP
