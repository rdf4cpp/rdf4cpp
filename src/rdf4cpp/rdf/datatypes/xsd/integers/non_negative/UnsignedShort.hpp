#ifndef RDF4CPP_XSD_UNSIGNED_SHORT_HPP
#define RDF4CPP_XSD_UNSIGNED_SHORT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedInt.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_unsigned_short> {
    using cpp_datatype = uint16_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_unsigned_short> {
    using supertype = xsd::UnsignedInt;
};

template<>
struct DatatypeNumericStubMapping<xsd_unsigned_short> {
    using numeric_impl_type = xsd::Integer;
};

template<>
capabilities::Default<xsd_unsigned_short>::cpp_type capabilities::Default<xsd_unsigned_short>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_unsigned_short>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept;

template<>
bool capabilities::Logical<xsd_unsigned_short>::effective_boolean_value(cpp_type const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_unsigned_short,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Subtype,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct UnsignedShort : registry::LiteralDatatypeImpl<registry::xsd_unsigned_short,
                                                     registry::capabilities::Logical,
                                                     registry::capabilities::NumericStub,
                                                     registry::capabilities::Subtype,
                                                     registry::capabilities::Comparable,
                                                     registry::capabilities::FixedId,
                                                     registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::UnsignedShort const xsd_unsigned_short_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_UNSIGNED_SHORT_HPP
