#ifndef RDF4CPP_XSD_UNSIGNED_INT_HPP
#define RDF4CPP_XSD_UNSIGNED_INT_HPP

#include <rdf4cpp/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/datatypes/xsd/integers/non_negative/UnsignedLong.hpp>

#include <cstdint>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_unsigned_int> {
   using cpp_datatype = uint32_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_unsigned_int> {
   using supertype = xsd::UnsignedLong;
};

template<>
struct DatatypeNumericStubMapping<xsd_unsigned_int> {
   using numeric_impl_type = xsd::Integer;
};

template<>
capabilities::Default<xsd_unsigned_int>::cpp_type capabilities::Default<xsd_unsigned_int>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_unsigned_int>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept;

template<>
bool capabilities::Logical<xsd_unsigned_int>::effective_boolean_value(cpp_type const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_unsigned_int,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Comparable,
                                           capabilities::Subtype,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::datatypes::registry


namespace rdf4cpp::datatypes::xsd {

struct UnsignedInt : registry::LiteralDatatypeImpl<registry::xsd_unsigned_int,
                                                   registry::capabilities::Logical,
                                                   registry::capabilities::NumericStub,
                                                   registry::capabilities::Comparable,
                                                   registry::capabilities::Subtype,
                                                   registry::capabilities::FixedId,
                                                   registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::datatypes::xsd


namespace rdf4cpp::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::UnsignedInt const xsd_unsigned_int_instance;

} // namespace rdf4cpp::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_UNSIGNED_INT_HPP
