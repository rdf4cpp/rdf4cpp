/**
 * @file Registers xsd:int with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_INT_HPP
#define RDF4CPP_XSD_INT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Long.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_int> {
    using cpp_datatype = int32_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_int> {
    using supertype = xsd::Long;
};

template<>
struct DatatypeNumericStubMapping<xsd_int> {
    using numeric_impl_type = xsd::Integer;
};

template<>
inline capabilities::Default<xsd_int>::cpp_type capabilities::Default<xsd_int>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
inline std::string capabilities::Default<xsd_int>::to_string(cpp_type const &value) {
    return util::to_chars(value);
}

template<>
inline bool capabilities::Logical<xsd_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Int : registry::LiteralDatatypeImpl<registry::xsd_int,
                                           registry::capabilities::Logical,
                                           registry::capabilities::NumericStub,
                                           registry::capabilities::Comparable,
                                           registry::capabilities::Subtype,
                                           registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_INT_HPP