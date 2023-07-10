#include <rdf4cpp/rdf/datatypes/xsd/time/Date.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_date>::cpp_type capabilities::Default<xsd_date>::from_string(std::string_view s) {
    auto year = parse_date_time_fragment<std::chrono::year, int, '-'>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-'>(s);
    auto tz = Timezone::try_parse(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0'>(tz.second);
    auto date = year / month / day;
    if (!date.ok())
        throw std::invalid_argument("invalid date");

    return std::make_pair(date, tz.first);
}

template<>
std::string capabilities::Default<xsd_date>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_date>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<std::chrono::year_month_day>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

static_assert(sizeof(std::chrono::year_month_day) * 8 <= storage::node::identifier::LiteralID::width);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_date>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value())
        return std::nullopt;
    return util::pack<storage::node::identifier::LiteralID>(value.first);
}

template<>
capabilities::Inlineable<xsd_date>::cpp_type capabilities::Inlineable<xsd_date>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<std::chrono::year_month_day>(inlined);
    return std::make_pair(i, std::nullopt);
}

template<>
TimePoint to_point_on_timeline<std::chrono::year_month_day>(std::chrono::year_month_day t) {
    return construct(t, TimePointReplacementTimeOfDay);
}

template<>
template<>
capabilities::Subtype<xsd_date>::super_cpp_type<0> capabilities::Subtype<xsd_date>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(to_point_on_timeline(value.first), value.second);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_date>::cpp_type, DynamicError> capabilities::Subtype<xsd_date>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
}

template struct LiteralDatatypeImpl<xsd_date,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;
}  // namespace rdf4cpp::rdf::datatypes::registry