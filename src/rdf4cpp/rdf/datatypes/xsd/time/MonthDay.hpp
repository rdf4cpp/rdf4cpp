#ifndef RDF4CPP_MONTHDAY_HPP
#define RDF4CPP_MONTHDAY_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_gMonthDay> {
    using cpp_datatype = std::pair<std::chrono::month_day, OptionalTimezone>;
};


template<>
capabilities::Default<xsd_gMonthDay>::cpp_type capabilities::Default<xsd_gMonthDay>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_gMonthDay>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_gMonthDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_gMonthDay,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

template<>
TimePoint to_timePoint<std::chrono::month_day>(std::chrono::month_day t);

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct GMonthDay : registry::LiteralDatatypeImpl<registry::xsd_gMonthDay,
                                            registry::capabilities::Comparable,
                                            registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::GMonthDay const xsd_GMonthDay_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_MONTHDAY_HPP
