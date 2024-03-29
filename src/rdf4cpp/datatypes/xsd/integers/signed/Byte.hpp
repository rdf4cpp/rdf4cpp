#ifndef RDF4CPP_XSD_BYTE_HPP
#define RDF4CPP_XSD_BYTE_HPP

#include <rdf4cpp/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/datatypes/xsd/integers/signed/Short.hpp>
#include <rdf4cpp/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
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
bool capabilities::Default<xsd_byte>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept;


template<>
bool capabilities::Logical<xsd_byte>::effective_boolean_value(cpp_type const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_byte,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Subtype,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::datatypes::registry


namespace rdf4cpp::datatypes::xsd {

struct Byte : registry::LiteralDatatypeImpl<registry::xsd_byte,
                                            registry::capabilities::Logical,
                                            registry::capabilities::NumericStub,
                                            registry::capabilities::Subtype,
                                            registry::capabilities::Comparable,
                                            registry::capabilities::FixedId,
                                            registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::datatypes::xsd


namespace rdf4cpp::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Byte const xsd_byte_instance;

} // namespace rdf4cpp::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_BYTE_HPP
