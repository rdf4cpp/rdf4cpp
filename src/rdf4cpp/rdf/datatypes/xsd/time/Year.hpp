#ifndef RDF4CPP_YEAR_HPP
#define RDF4CPP_YEAR_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Timezone.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Date.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_gYear> {
    using cpp_datatype = std::pair<std::chrono::year, OptionalTimezone>;
};
template<>
struct DatatypeSupertypeMapping<xsd_gYear> {
    using supertype = xsd::Date;
};


template<>
capabilities::Default<xsd_gYear>::cpp_type capabilities::Default<xsd_gYear>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_gYear>::to_canonical_string(const cpp_type &value) noexcept;



template<>
std::partial_ordering capabilities::Comparable<xsd_gYear>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gYear>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_gYear>::cpp_type capabilities::Inlineable<xsd_gYear>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

template<>
template<>
capabilities::Subtype<xsd_gYear>::super_cpp_type<0> capabilities::Subtype<xsd_gYear>::into_supertype<0>(cpp_type const &value) noexcept;

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gYear>::cpp_type, DynamicError> capabilities::Subtype<xsd_gYear>::from_supertype<0>(super_cpp_type<0> const &value) noexcept;

extern template struct LiteralDatatypeImpl<xsd_gYear,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable,
                                           capabilities::Subtype>;

template<>
TimePoint to_point_on_timeline<std::chrono::year>(std::chrono::year t);

}

namespace rdf4cpp::rdf::datatypes::xsd {

struct GYear : registry::LiteralDatatypeImpl<registry::xsd_gYear,
                                             registry::capabilities::Comparable,
                                             registry::capabilities::FixedId,
                                             registry::capabilities::Inlineable,
                                             registry::capabilities::Subtype> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::GYear const xsd_gYear_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_YEAR_HPP
