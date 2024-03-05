#include "Year.hpp"

#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_gYear>::cpp_type capabilities::Default<xsd_gYear>::from_string(std::string_view s) {
    auto tz = rdf4cpp::util::Timezone::parse_optional(s);
    auto year = registry::util::parse_date_time_fragment<std::chrono::year, int, '\0', identifier>(s);
    return std::make_pair(year, tz);
}

template<>
bool capabilities::Default<xsd_gYear>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    auto str = std::format("{:%Y}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();

    return writer::write_str(str, writer);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gYear>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    auto year_to_tp = [](std::chrono::year t) noexcept -> rdf4cpp::util::TimePoint {
        return rdf4cpp::util::construct(t / rdf4cpp::util::TimePointReplacementDate.month() / rdf4cpp::util::TimePointReplacementDate.day(), rdf4cpp::util::TimePointReplacementTimeOfDay);
    };
    return registry::util::compare_time_points(year_to_tp(lhs.first), lhs.second, year_to_tp(rhs.first), rhs.second);
}

using IHelp = registry::util::InliningHelper<int16_t>;
static_assert(sizeof(std::chrono::year) == 2);
static_assert(sizeof(IHelp) * 8 < storage::identifier::LiteralID::width);

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_gYear>::try_into_inlined(cpp_type const &value) noexcept {
    IHelp i{static_cast<int16_t>(static_cast<int>(value.first)), value.second};
    return util::pack<storage::identifier::LiteralID>(i);
}

template<>
capabilities::Inlineable<xsd_gYear>::cpp_type capabilities::Inlineable<xsd_gYear>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<IHelp>(inlined);
    return std::make_pair(std::chrono::year{i.time_value}, i.decode_tz());
}

template<>
template<>
capabilities::Subtype<xsd_gYear>::super_cpp_type<0> capabilities::Subtype<xsd_gYear>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(value.first / rdf4cpp::util::TimePointReplacementDate.month() / rdf4cpp::util::TimePointReplacementDate.day(), value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gYear>::cpp_type, DynamicError> capabilities::Subtype<xsd_gYear>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(value.first.year(), value.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_gYear,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;

}
