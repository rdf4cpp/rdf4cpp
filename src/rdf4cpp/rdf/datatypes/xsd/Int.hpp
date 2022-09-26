/**
 * @file Registers xsd:int with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INT_HPP
#define RDF4CPP_XSD_INT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Integer.hpp>

#include <charconv>
#include <cstdint>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {
/**
 * Defines the mapping between the LiteralDatatype IRI and the C++ datatype.
 */
template<>
struct DatatypeMapping<xsd_int> {
    using cpp_datatype = int32_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_int> {
    using supertype = xsd::Integer;
};

template<>
struct DatatypeNumericStubMapping<xsd_int> {
    using numeric_impl_type = xsd::Integer;
};

/**
 * Specialisation of from_string template function.
 */
template<>
inline capabilities::Default<xsd_int>::cpp_type capabilities::Default<xsd_int>::from_string(std::string_view s) {

    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    cpp_type value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error{"XSD Parsing Error"};
    } else {
        return value;
    }
}

template<>
inline bool capabilities::Logical<xsd_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::integer
 */
struct Int : registry::LiteralDatatypeImpl<registry::xsd_int,
                                           registry::capabilities::Logical,
                                           registry::capabilities::NumericStub,
                                           registry::capabilities::Comparable,
                                           registry::capabilities::Subtype,
                                           registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_INT_HPP
