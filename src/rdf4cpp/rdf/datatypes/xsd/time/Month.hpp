#ifndef RDF4CPP_MONTH_HPP
#define RDF4CPP_MONTH_HPP

#include <chrono>

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/util/Timezone.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/time/Date.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_gMonth> {
    using cpp_datatype = std::pair<std::chrono::month, rdf::util::OptionalTimezone>;
};
template<>
struct DatatypeSupertypeMapping<xsd_gMonth> {
    using supertype = xsd::Date;
};


template<>
capabilities::Default<xsd_gMonth>::cpp_type capabilities::Default<xsd_gMonth>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_gMonth>::to_canonical_string(const cpp_type &value) noexcept;



template<>
std::partial_ordering capabilities::Comparable<xsd_gMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gMonth>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_gMonth>::cpp_type capabilities::Inlineable<xsd_gMonth>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept;

template<>
template<>
capabilities::Subtype<xsd_gMonth>::super_cpp_type<0> capabilities::Subtype<xsd_gMonth>::into_supertype<0>(cpp_type const &value) noexcept;

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gMonth>::cpp_type, DynamicError> capabilities::Subtype<xsd_gMonth>::from_supertype<0>(super_cpp_type<0> const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_gMonth,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable,
                                           capabilities::Subtype>;

}

namespace rdf4cpp::rdf::datatypes::xsd {

struct GMonth : registry::LiteralDatatypeImpl<registry::xsd_gMonth,
                                              registry::capabilities::Comparable,
                                              registry::capabilities::FixedId,
                                              registry::capabilities::Inlineable,
                                              registry::capabilities::Subtype> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::GMonth const xsd_gMonth_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_MONTH_HPP
