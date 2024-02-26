#include "DateTime.hpp"

#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_dateTime>::cpp_type capabilities::Default<xsd_dateTime>::from_string(std::string_view s) {
    using namespace rdf::datatypes::registry::util;
    auto year = parse_date_time_fragment<std::chrono::year, int, '-', identifier>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-', identifier>(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, 'T', identifier>(s);
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':', identifier>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':', identifier>(s);
    auto tz = rdf::util::Timezone::parse_optional(s);
    std::chrono::milliseconds ms = parse_milliseconds<identifier>(s);
    auto date = year / month / day;
    if (registry::relaxed_parsing_mode && !date.ok())
        date = normalize(date);
    if (!date.ok()) {
        throw std::runtime_error("invalid date");
    }
    if (!registry::relaxed_parsing_mode) {
        if (minutes < std::chrono::minutes(0) || minutes > std::chrono::hours(1))
            throw std::runtime_error{"minutes out of range"};
        if (hours < std::chrono::hours(0) || hours > std::chrono::days(1))
            throw std::runtime_error{"hours out of range"};
        if (ms < std::chrono::seconds(0) || ms > std::chrono::minutes(1))
            throw std::runtime_error{"seconds out of range"};
    }
    auto time = hours + minutes + ms;
    if (!registry::relaxed_parsing_mode) {
        if (time == std::chrono::hours{24}) {
            date = std::chrono::year_month_day{std::chrono::local_days{date} + std::chrono::days{1}};
            if (!date.ok())
                throw std::runtime_error("invalid date");
            time = std::chrono::hours{0};
        } else if (time > std::chrono::hours{24}) {
            throw std::runtime_error{"invalid time of day"};
        }
    }

    return std::make_pair(rdf::util::construct(date, time), tz);
}

template<>
bool capabilities::Default<xsd_dateTime>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    auto str = std::format("{:%Y-%m-%dT%H:%M:%S}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();

    return writer::write_str(str, buffer, cursor, flush);
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
    return std::make_pair(rdf::util::TimePoint{std::chrono::seconds{util::unpack_integral<int64_t>(inlined)}}, std::nullopt);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTime>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return rdf::datatypes::registry::util::compare_time_points(lhs.first, lhs.second, rhs.first, rhs.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_dateTime,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::rdf::datatypes::registry