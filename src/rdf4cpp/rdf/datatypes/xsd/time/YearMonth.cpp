#include <rdf4cpp/rdf/datatypes/xsd/time/YearMonth.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gYearMonth>::cpp_type capabilities::Default<xsd_gYearMonth>::from_string(std::string_view s) {
    auto year = parse_date_time_fragment<std::chrono::year, int, '-'>(s);
    auto tz = Timezone::try_parse(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '\0'>(tz.second);
    auto date = year / month;
    if (!date.ok())
        throw std::invalid_argument("invalid date");

    return std::make_pair(date, tz.first);
}

template<>
std::string capabilities::Default<xsd_gYearMonth>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gYearMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<std::chrono::year_month>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

template<>
TimePoint to_point_on_timeline<std::chrono::year_month>(std::chrono::year_month t) {
    return construct(t / std::chrono::last, TimePointReplacementTimeOfDay);
}

template struct LiteralDatatypeImpl<xsd_gYearMonth,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry