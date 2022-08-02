/**
 * @file Registers xsd:float with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_FLOAT_HPP
#define RDF4CPP_XSD_FLOAT_HPP


#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <charconv>
#include <sstream>
#include <stdexcept>

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
inline capabilities::Default<xsd_float>::cpp_type capabilities::Default<xsd_float>::from_string(std::string_view s) {

    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    cpp_type value;
    std::from_chars_result const res = std::from_chars(s.data(), s.data() + s.size(), value, std::chars_format::general);

    if (res.ptr != s.data() + s.size()) {
        // parsing did not reach end of string => it contains invalid characters
        throw std::runtime_error{ "XSD Parsing Error" };
    }

    return value;
}

/**
 * Specialisation of to_string template function.
 */
template<>
inline std::string capabilities::Default<xsd_float>::to_string(const cpp_type &value) {

    std::ostringstream str_os;
    // Set Fixed -Point Notation
    str_os << std::fixed;
    str_os << value;
    // Get string from output string stream
    std::string str = str_os.str();
    return str;
}

template<>
inline bool capabilities::Logical<xsd_float>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.f;
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::float
 */
using Float = registry::LiteralDatatypeImpl<registry::xsd_float,
                                            registry::capabilities::Logical,
                                            registry::capabilities::Numeric>;
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_FLOAT_HPP
