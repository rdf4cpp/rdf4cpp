#include <rdf4cpp/rdf/datatypes/xsd/time/DateTimeStamp.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_dateTimeStamp>::cpp_type capabilities::Default<xsd_dateTimeStamp>::from_string(std::string_view s) {
    auto year = parse_date_time_fragment<std::chrono::year, int, '-'>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-'>(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, 'T'>(s);
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':'>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':'>(s);
    auto p = s.find_first_of(Timezone::begin_tokens);
    if (p == 0)
        throw std::runtime_error{"invalid seconds"};
    if (p == std::string::npos)
        throw std::invalid_argument{"missing timezone"};
    auto tz = Timezone::parse(s.substr(p));
    std::chrono::milliseconds ms = parse_milliseconds(s.substr(0, p));
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

    return ZonedTime{tz, construct(date, time)};
}

template<>
std::string capabilities::Default<xsd_dateTimeStamp>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m-%dT%H:%M:%S%Z}", value);
    return str;
}

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_dateTimeStamp>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.get_time_zone() != xsd::DateTimeStamp::inlining_default_timezone)
        return std::nullopt;
    auto tp_sec = std::chrono::floor<std::chrono::seconds>(value.get_sys_time());
    if ((value.get_sys_time() - tp_sec).count() != 0)
        return std::nullopt;
    auto s = static_cast<int64_t>(tp_sec.time_since_epoch().count());
    return util::try_pack_integral<storage::node::identifier::LiteralID>(s);
}

template<>
capabilities::Inlineable<xsd_dateTimeStamp>::cpp_type capabilities::Inlineable<xsd_dateTimeStamp>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    std::integral auto i = util::unpack_integral<int64_t>(inlined);
    return registry::ZonedTime{xsd::DateTimeStamp::inlining_default_timezone, registry::TimePointSys{std::chrono::seconds{i}}};
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTimeStamp>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs.get_sys_time() <=> rhs.get_sys_time();
}

template<>
template<>
capabilities::Subtype<xsd_dateTimeStamp>::super_cpp_type<0> capabilities::Subtype<xsd_dateTimeStamp>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(value.get_local_time(), value.get_time_zone());
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_dateTimeStamp>::cpp_type, DynamicError> capabilities::Subtype<xsd_dateTimeStamp>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    if (!value.second.has_value())
        return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
    return ZonedTime{*value.second, value.first};
}

template struct LiteralDatatypeImpl<xsd_dateTimeStamp,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Subtype,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::rdf::datatypes::registry