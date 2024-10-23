#ifndef RDF4CPP_YEARMONTH_HPP
#define RDF4CPP_YEARMONTH_HPP

#include <chrono>

#include <rdf4cpp/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/Timezone.hpp>
#include <rdf4cpp/datatypes/xsd/time/Date.hpp>
#include <dice/hash.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
struct DatatypeMapping<xsd_gYearMonth> {
    using cpp_datatype = std::pair<rdf4cpp::RDFYearMonth, rdf4cpp::OptionalTimezone>;
};
template<>
struct DatatypeSupertypeMapping<xsd_gYearMonth> {
    using supertype = xsd::Date;
};


template<>
capabilities::Default<xsd_gYearMonth>::cpp_type capabilities::Default<xsd_gYearMonth>::from_string(std::string_view s);

template<>
bool capabilities::Default<xsd_gYearMonth>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept;

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_gYearMonth>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_gYearMonth>::cpp_type capabilities::Inlineable<xsd_gYearMonth>::from_inlined(storage::identifier::LiteralID inlined) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_gYearMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
template<>
capabilities::Subtype<xsd_gYearMonth>::super_cpp_type<0> capabilities::Subtype<xsd_gYearMonth>::into_supertype<0>(cpp_type const &value) noexcept;

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gYearMonth>::cpp_type, DynamicError> capabilities::Subtype<xsd_gYearMonth>::from_supertype<0>(super_cpp_type<0> const &value) noexcept;
#endif

extern template struct LiteralDatatypeImpl<xsd_gYearMonth,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable,
                                           capabilities::Subtype>;

}  // namespace rdf4cpp::datatypes::registry

namespace rdf4cpp::datatypes::xsd {

struct GYearMonth : registry::LiteralDatatypeImpl<registry::xsd_gYearMonth,
                                                  registry::capabilities::Comparable,
                                                  registry::capabilities::FixedId,
                                                  registry::capabilities::Inlineable,
                                                  registry::capabilities::Subtype> {};

}  // namespace rdf4cpp::datatypes::xsd


namespace rdf4cpp::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::GYearMonth const xsd_gYearMonth_instance;

} // namespace rdf4cpp::datatypes::registry::instantiation_detail

#ifndef DOXYGEN_PARSER
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, std::chrono::year_month> {
    static size_t dice_hash(std::chrono::year_month const &x) noexcept {
        auto y = static_cast<int>(x.year());
        auto m = static_cast<unsigned int>(x.month());
        return dice::hash::dice_hash_templates<Policy>::dice_hash(std::tie(y, m));
    }
};
#endif

#endif  //RDF4CPP_YEARMONTH_HPP
