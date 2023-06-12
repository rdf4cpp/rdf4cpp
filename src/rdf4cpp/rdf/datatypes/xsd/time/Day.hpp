#ifndef RDF4CPP_DAY_HPP
#define RDF4CPP_DAY_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_gDay> {
    using cpp_datatype = std::pair<std::chrono::day, OptionalTimezone>;
};


template<>
capabilities::Default<xsd_gDay>::cpp_type capabilities::Default<xsd_gDay>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_gDay>::to_canonical_string(const cpp_type &value) noexcept;



template<>
std::partial_ordering capabilities::Comparable<xsd_gDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

extern template struct LiteralDatatypeImpl<xsd_gDay,
                                           capabilities::Comparable,
                                           capabilities::FixedId>;

}

namespace rdf4cpp::rdf::datatypes::xsd {

struct GDay : registry::LiteralDatatypeImpl<registry::xsd_gDay,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::GDay const xsd_gDay_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_DAY_HPP
