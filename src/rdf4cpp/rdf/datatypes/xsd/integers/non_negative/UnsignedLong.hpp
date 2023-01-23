#ifndef RDF4CPP_XSD_UNSIGNED_LONG_HPP
#define RDF4CPP_XSD_UNSIGNED_LONG_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/NonNegativeInteger.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_unsigned_long> {
    using cpp_datatype = uint64_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_unsigned_long> {
    using supertype = xsd::NonNegativeInteger;
};

template<>
struct DatatypeNumericStubMapping<xsd_unsigned_long> {
    using numeric_impl_type = xsd::Integer;
};

template<>
capabilities::Default<xsd_unsigned_long>::cpp_type capabilities::Default<xsd_unsigned_long>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_unsigned_long>::to_canonical_string(cpp_type const &value) noexcept;

template<>
bool capabilities::Logical<xsd_unsigned_long>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
nonstd::expected<capabilities::Default<xsd_unsigned_long>::cpp_type, DynamicError> capabilities::Subtype<xsd_unsigned_long>::from_supertype(super_cpp_type const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_unsigned_long,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Subtype,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct UnsignedLong : registry::LiteralDatatypeImpl<registry::xsd_unsigned_long,
                                                    registry::capabilities::Logical,
                                                    registry::capabilities::NumericStub,
                                                    registry::capabilities::Subtype,
                                                    registry::capabilities::Comparable,
                                                    registry::capabilities::FixedId,
                                                    registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::UnsignedLong const xsd_unsigned_long_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_UNSIGNED_LONG_HPP
