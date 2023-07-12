#ifndef RDF4CPP_DATETIMESTAMP_HPP
#define RDF4CPP_DATETIMESTAMP_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/DateTime.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_dateTimeStamp> {
    using cpp_datatype = ZonedTime;
};
template<>
struct DatatypeSupertypeMapping<xsd_dateTimeStamp> {
    using supertype = xsd::DateTime;
};


template<>
capabilities::Default<xsd_dateTimeStamp>::cpp_type capabilities::Default<xsd_dateTimeStamp>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_dateTimeStamp>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_dateTimeStamp>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
template<>
capabilities::Subtype<xsd_dateTimeStamp>::super_cpp_type<0> capabilities::Subtype<xsd_dateTimeStamp>::into_supertype<0>(cpp_type const &value) noexcept;

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_dateTimeStamp>::cpp_type, DynamicError> capabilities::Subtype<xsd_dateTimeStamp>::from_supertype<0>(super_cpp_type<0> const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_dateTimeStamp,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Subtype>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct DateTimeStamp : registry::LiteralDatatypeImpl<registry::xsd_dateTimeStamp,
                                                     registry::capabilities::Comparable,
                                                     registry::capabilities::FixedId,
                                                     registry::capabilities::Subtype> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::DateTimeStamp const xsd_DateTimeStamp_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_DATETIMESTAMP_HPP
