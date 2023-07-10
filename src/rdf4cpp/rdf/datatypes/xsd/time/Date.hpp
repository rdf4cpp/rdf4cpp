#ifndef RDF4CPP_DATE_HPP
#define RDF4CPP_DATE_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_date> {
    using cpp_datatype = std::pair<std::chrono::year_month_day, OptionalTimezone>;
};


template<>
capabilities::Default<xsd_date>::cpp_type capabilities::Default<xsd_date>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_date>::to_canonical_string(const cpp_type &value) noexcept;


template<>
std::partial_ordering capabilities::Comparable<xsd_date>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_date>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_date>::cpp_type capabilities::Inlineable<xsd_date>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

extern template struct LiteralDatatypeImpl<xsd_date,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

template<>
TimePoint to_point_on_timeline<std::chrono::year_month_day>(std::chrono::year_month_day t);

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct Date : registry::LiteralDatatypeImpl<registry::xsd_date,
                                            registry::capabilities::Comparable,
                                            registry::capabilities::FixedId,
                                            registry::capabilities::Inlineable> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::Date const xsd_Date_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_DATE_HPP
