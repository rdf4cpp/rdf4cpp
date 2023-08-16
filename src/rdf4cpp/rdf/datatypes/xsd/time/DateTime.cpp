#include <rdf4cpp/rdf/datatypes/xsd/time/DateTime.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_dateTime>::cpp_type capabilities::Default<xsd_dateTime>::from_string(std::string_view s) {
    auto year = parse_date_time_fragment<std::chrono::year, int, '-'>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-'>(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, 'T'>(s);
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':'>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':'>(s);
    auto tz = Timezone::try_parse(s);
    std::chrono::milliseconds ms = parse_milliseconds(tz.second);
    auto date = year / month / day;
    if (!date.ok())
        throw std::invalid_argument("invalid date");
    if (minutes < std::chrono::minutes(0) || minutes > std::chrono::hours(1))
        throw std::invalid_argument{"minutes out of range"};
    if (hours < std::chrono::hours(0) || hours > std::chrono::days(1))
        throw std::invalid_argument{"hours out of range"};
    if (ms < std::chrono::seconds(0) || ms > std::chrono::minutes(1))
        throw std::invalid_argument{"seconds out of range"};
    auto time = hours + minutes + ms;
    if (time == std::chrono::hours{24}) {
        date = std::chrono::year_month_day{std::chrono::local_days{date} + std::chrono::days{1}};
        if (!date.ok())
            throw std::invalid_argument("invalid date");
        time = std::chrono::hours{0};
    } else if (time > std::chrono::hours{24}) {
        throw std::invalid_argument{"invalid time of day"};
    }

    return std::make_pair(construct(date, time), tz.first);
}

template<>
std::string capabilities::Default<xsd_dateTime>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m-%dT%H:%M:%S}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_dateTime>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value())
        return std::nullopt;
    auto tp_sec = std::chrono::floor<std::chrono::seconds>(value.first);
    if ((value.first - tp_sec).count() != 0)
        return std::nullopt;
    auto s = static_cast<int64_t>(tp_sec.time_since_epoch().count());
    return util::try_pack_integral<storage::node::identifier::LiteralID>(s);
}

template<>
capabilities::Inlineable<xsd_dateTime>::cpp_type capabilities::Inlineable<xsd_dateTime>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    return std::make_pair(TimePoint{std::chrono::seconds{util::unpack_integral<int64_t>(inlined)}}, std::nullopt);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTime>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<TimePoint>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

template struct LiteralDatatypeImpl<xsd_dateTime,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::rdf::datatypes::registry