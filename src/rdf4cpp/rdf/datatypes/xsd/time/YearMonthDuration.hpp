#ifndef RDF4CPP_YEARMONTHDURATION_HPP
#define RDF4CPP_YEARMONTHDURATION_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/util/Timezone.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Duration.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_yearMonthDuration> {
    using cpp_datatype = std::chrono::months;
};
template<>
struct DatatypeSupertypeMapping<xsd_yearMonthDuration> {
    using supertype = xsd::Duration;
};


template<>
capabilities::Default<xsd_yearMonthDuration>::cpp_type capabilities::Default<xsd_yearMonthDuration>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_yearMonthDuration>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_yearMonthDuration>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_yearMonthDuration>::cpp_type capabilities::Inlineable<xsd_yearMonthDuration>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_yearMonthDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
template<>
capabilities::Subtype<xsd_yearMonthDuration>::super_cpp_type<0> capabilities::Subtype<xsd_yearMonthDuration>::into_supertype<0>(cpp_type const &value) noexcept;

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_yearMonthDuration>::cpp_type, DynamicError> capabilities::Subtype<xsd_yearMonthDuration>::from_supertype<0>(super_cpp_type<0> const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_yearMonthDuration,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable,
                                           capabilities::Subtype>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct YearMonthDuration : registry::LiteralDatatypeImpl<registry::xsd_yearMonthDuration,
                                                         registry::capabilities::Comparable,
                                                         registry::capabilities::FixedId,
                                                         registry::capabilities::Inlineable,
                                                         registry::capabilities::Subtype> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::YearMonthDuration const xsd_YearMonthDuration_instance;

}  // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_YEARMONTHDURATION_HPP
