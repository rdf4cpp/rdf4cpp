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
    if (p == std::string::npos)
        throw std::invalid_argument{"missing timezone"};
    auto tz = Timezone::parse(s.substr(p));
    std::chrono::milliseconds ms = parse_milliseconds(s.substr(0, p));
    auto date = year / month / day;
    if (!date.ok())
       throw std::invalid_argument("invalid date");
    auto time = hours + minutes + ms;
    if (time > std::chrono::hours{24})
        throw std::invalid_argument{"invalid time of day"};

    return ZonedTime{tz, construct(date, time)};
}

template<>
std::string capabilities::Default<xsd_dateTimeStamp>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m-%dT%H:%M:%S%Z}", value);
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTimeStamp>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs.get_sys_time() <=> rhs.get_sys_time();
}

template struct LiteralDatatypeImpl<xsd_dateTimeStamp,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry