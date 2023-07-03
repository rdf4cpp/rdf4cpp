#ifndef RDF4CPP_YEARMONTHDURATION_HPP
#define RDF4CPP_YEARMONTHDURATION_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_yearMonthDuration> {
    using cpp_datatype = std::chrono::months;
};


template<>
capabilities::Default<xsd_yearMonthDuration>::cpp_type capabilities::Default<xsd_yearMonthDuration>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_yearMonthDuration>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_yearMonthDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_yearMonthDuration,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct YearMonthDuration : registry::LiteralDatatypeImpl<registry::xsd_yearMonthDuration,
                                                         registry::capabilities::Comparable,
                                                         registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::YearMonthDuration const xsd_YearMonthDuration_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_YEARMONTHDURATION_HPP
