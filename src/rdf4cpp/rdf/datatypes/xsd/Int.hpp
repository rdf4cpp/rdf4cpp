/**
 * @file Registers xsd:int with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INT_HPP
#define RDF4CPP_XSD_INT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <charconv>
#include <cstdint>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {
/*
 * Name of the datatype. This is kept so that we won't need to type it over and over again.
 */
constexpr static registry::ConstexprString xsd_int{"http://www.w3.org/2001/XMLSchema#int"};

/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_int> {
    using cpp_datatype = int32_t;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline LiteralDatatypeImpl<xsd_int>::cpp_type LiteralDatatypeImpl<xsd_int>::from_string(std::string_view s) {

    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    cpp_type value;
    std::from_chars_result const res = std::from_chars(s.data(), s.data() + s.size(), value, 10);

    if (res.ptr != s.data() + s.size()) {
        // parsing did not reach end of string => it contains invalid characters
        throw std::runtime_error{ "XSD Parsing Error" };
    }

    return value;
}
} // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

/**
 * Implementation of xsd::int
 */
using Int = registry::LiteralDatatypeImpl<registry::xsd_int>;

}  // namespace rdf4cpp::rdf::datatypes::xsd
#endif  //RDF4CPP_XSD_INT_HPP
