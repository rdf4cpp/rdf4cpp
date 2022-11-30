#ifndef RDF4CPP_XSD_SHORT_HPP
#define RDF4CPP_XSD_SHORT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Int.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_short> {
    using cpp_datatype = int16_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_short> {
    using supertype = xsd::Int;
};

template<>
struct DatatypeNumericStubMapping<xsd_short> {
    using numeric_impl_type = xsd::Integer;
};

template<>
capabilities::Default<xsd_short>::cpp_type capabilities::Default<xsd_short>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_short>::to_string(cpp_type const &value) noexcept;


template<>
bool capabilities::Logical<xsd_short>::effective_boolean_value(cpp_type const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_short,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Subtype,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Short : registry::LiteralDatatypeImpl<registry::xsd_short,
                                             registry::capabilities::Logical,
                                             registry::capabilities::NumericStub,
                                             registry::capabilities::Subtype,
                                             registry::capabilities::Comparable,
                                             registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Short const xsd_short_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_SHORT_HPP
