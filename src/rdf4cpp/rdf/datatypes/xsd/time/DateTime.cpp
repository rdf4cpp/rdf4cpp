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
    auto time = hours + minutes + ms;
    if (time > std::chrono::hours{24})
        throw std::invalid_argument{"invalid time of day"};

    return std::make_pair(DateTime{date, time}, tz.first);
}

template<>
std::string capabilities::Default<xsd_dateTime>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m-%d}T{:%H:%M:%S}", value.first.date, value.first.time_of_day);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTime>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs.first <=> rhs.first;
}

template struct LiteralDatatypeImpl<xsd_dateTime,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry