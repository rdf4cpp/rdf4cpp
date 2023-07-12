#include <rdf4cpp/rdf/datatypes/xsd/time/MonthDay.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gMonthDay>::cpp_type capabilities::Default<xsd_gMonthDay>::from_string(std::string_view s) {
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-'>(s);
    auto tz = Timezone::try_parse(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0'>(tz.second);
    auto date = month / day;
    if (!date.ok())
        throw std::invalid_argument("invalid date");

    return std::make_pair(date, tz.first);
}

template<>
std::string capabilities::Default<xsd_gMonthDay>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

using IHelp = InliningHelper<std::chrono::month_day>;
static_assert(sizeof(std::chrono::month_day) == 2);
static_assert(sizeof(IHelp) * 8 < storage::node::identifier::LiteralID::width);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_gMonthDay>::try_into_inlined(cpp_type const &value) noexcept {
    IHelp i{value.first, value.second};
    return util::pack<storage::node::identifier::LiteralID>(i);
}

template<>
capabilities::Inlineable<xsd_gMonthDay>::cpp_type capabilities::Inlineable<xsd_gMonthDay>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<IHelp>(inlined);
    return std::make_pair(i.time_value, i.decode_tz());
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gMonthDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<std::chrono::month_day>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

template<>
template<>
capabilities::Subtype<xsd_gMonthDay>::super_cpp_type<0> capabilities::Subtype<xsd_gMonthDay>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(TimePointReplacementDate.year() / value.first, value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_gMonthDay>::cpp_type, DynamicError> capabilities::Subtype<xsd_gMonthDay>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return std::make_pair(value.first.month() / value.first.day(), value.second);
}

template struct LiteralDatatypeImpl<xsd_gMonthDay,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;

template<>
TimePoint to_point_on_timeline<std::chrono::month_day>(std::chrono::month_day t) {
    return construct(TimePointReplacementDate.year() / t, TimePointReplacementTimeOfDay);
}

}  // namespace rdf4cpp::rdf::datatypes::registry