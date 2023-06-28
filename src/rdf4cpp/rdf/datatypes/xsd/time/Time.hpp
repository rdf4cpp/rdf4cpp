#ifndef RDF4CPP_TIME_HPP
#define RDF4CPP_TIME_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_time> {
    using cpp_datatype = std::pair<std::chrono::milliseconds, OptionalTimezone>;
};


template<>
capabilities::Default<xsd_time>::cpp_type capabilities::Default<xsd_time>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_time>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_time>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_time,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

template<>
TimePoint to_point_on_timeline<std::chrono::milliseconds>(std::chrono::milliseconds t);

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct Time : registry::LiteralDatatypeImpl<registry::xsd_time,
                                            registry::capabilities::Comparable,
                                            registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Time const xsd_Time_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_TIME_HPP
