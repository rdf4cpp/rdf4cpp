#ifndef RDF4CPP_XSD_BYTE_HPP
#define RDF4CPP_XSD_BYTE_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Short.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_byte> {
    using cpp_datatype = int8_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_byte> {
    using supertype = xsd::Short;
};

template<>
struct DatatypeNumericStubMapping<xsd_byte> {
    using numeric_impl_type = xsd::Integer;
};

template<>
capabilities::Default<xsd_byte>::cpp_type capabilities::Default<xsd_byte>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_byte>::to_string(cpp_type const &value);


template<>
bool capabilities::Logical<xsd_byte>::effective_boolean_value(cpp_type const &value) noexcept;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Byte : registry::LiteralDatatypeImpl<registry::xsd_byte,
                                            registry::capabilities::Logical,
                                            registry::capabilities::NumericStub,
                                            registry::capabilities::Subtype,
                                            registry::capabilities::Comparable,
                                            registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_BYTE_HPP
