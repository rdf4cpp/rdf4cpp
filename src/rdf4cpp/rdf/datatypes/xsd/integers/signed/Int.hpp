#ifndef RDF4CPP_XSD_INT_HPP
#define RDF4CPP_XSD_INT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Long.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
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
capabilities::Default<xsd_int>::cpp_type capabilities::Default<xsd_int>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_int>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept;

template<>
bool capabilities::Logical<xsd_int>::effective_boolean_value(cpp_type const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_int,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Comparable,
                                           capabilities::Subtype,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Int : registry::LiteralDatatypeImpl<registry::xsd_int,
                                           registry::capabilities::Logical,
                                           registry::capabilities::NumericStub,
                                           registry::capabilities::Comparable,
                                           registry::capabilities::Subtype,
                                           registry::capabilities::FixedId,
                                           registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Int const xsd_int_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_INT_HPP
