#ifndef RDF4CPP_DAYTIMEDURATION_HPP
#define RDF4CPP_DAYTIMEDURATION_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_dayTimeDuration> {
    using cpp_datatype = std::chrono::milliseconds;
};


template<>
capabilities::Default<xsd_dayTimeDuration>::cpp_type capabilities::Default<xsd_dayTimeDuration>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_dayTimeDuration>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_dayTimeDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_dayTimeDuration,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct DayTimeDuration : registry::LiteralDatatypeImpl<registry::xsd_dayTimeDuration,
                                                       registry::capabilities::Comparable,
                                                       registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::DayTimeDuration const xsd_DaysTimeDuration_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_DAYTIMEDURATION_HPP
