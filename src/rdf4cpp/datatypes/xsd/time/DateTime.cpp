#include "DateTime.hpp"

#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_dateTime>::cpp_type capabilities::Default<xsd_dateTime>::from_string(std::string_view s) {
    using namespace datatypes::registry::util;
    auto year = parse_date_time_fragment<std::chrono::year, int, '-', identifier>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-', identifier>(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, 'T', identifier>(s);
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':', identifier>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':', identifier>(s);
    auto tz = rdf4cpp::Timezone::parse_optional(s, identifier);
    std::chrono::milliseconds ms = parse_milliseconds<identifier>(s);
    auto date = year / month / day;
    if (registry::relaxed_parsing_mode && !date.ok()) {
        date = normalize(date);
    }
    if (!date.ok()) {
        throw InvalidNode(std::format("{} parsing error: {:%Y-%m-%d} is invalid", identifier, date));
    }
    if (!registry::relaxed_parsing_mode) {
        if (minutes < std::chrono::minutes(0) || minutes > std::chrono::hours(1)) {
            throw InvalidNode(std::format("{} parsing error: minutes out of range", identifier));
        }
        if (hours < std::chrono::hours(0) || hours > std::chrono::days(1)) {
            throw InvalidNode(std::format("{} parsing error: hours out of range", identifier));
        }
        if (ms < std::chrono::seconds(0) || ms > std::chrono::minutes(1)) {
            throw InvalidNode(std::format("{} parsing error: seconds out of range", identifier));
        }
    }
    auto time = hours + minutes + ms;
    if (!registry::relaxed_parsing_mode) {
        if (time == std::chrono::hours{24}) {
            date = std::chrono::year_month_day{std::chrono::local_days{date} + std::chrono::days{1}};
            if (!date.ok()) {
                throw InvalidNode(std::format("{} parsing error: {:%Y-%m-%d} is invalid", identifier, date));
            }
            time = std::chrono::hours{0};
        } else if (time > std::chrono::hours{24}) {
            throw InvalidNode(std::format("{} parsing error: invalid time of day", identifier));
        }
    }

    return std::make_pair(rdf4cpp::util::construct_timepoint(date, time), tz);
}

template<>
bool capabilities::Default<xsd_dateTime>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    //year,-,month,-,day, T, hours,:,min,:,sec, tz
    std::array<char, registry::util::chrono_max_canonical_string_chars::year + 1 +
                             registry::util::chrono_max_canonical_string_chars::month + 1 +
                             registry::util::chrono_max_canonical_string_chars::day + 1 +
                             registry::util::chrono_max_canonical_string_chars::hours + 1 +
                             registry::util::chrono_max_canonical_string_chars::minutes + 1 +
                             registry::util::chrono_max_canonical_string_chars::seconds + Timezone::max_canonical_string_chars>
            buff;
    char *it = std::format_to(buff.data(), "{:%Y-%m-%dT%H:%M:%S}", value.first);
    it = util::canonical_seconds_remove_empty_millis(it);
    if (value.second.has_value()) {
        it = value.second->to_canonical_string(it);
    }
    size_t const len = it - buff.data();
    assert(len <= buff.size());
    return writer::write_str(std::string_view(buff.data(), len), writer);
}

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_dateTime>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.second.has_value())
        return std::nullopt;
    auto tp_sec = std::chrono::floor<std::chrono::seconds>(value.first);
    if ((value.first - tp_sec).count() != 0)
        return std::nullopt;
    auto s = static_cast<int64_t>(tp_sec.time_since_epoch().count());
    return util::try_pack_integral<storage::identifier::LiteralID>(s);
}

template<>
capabilities::Inlineable<xsd_dateTime>::cpp_type capabilities::Inlineable<xsd_dateTime>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    return std::make_pair(rdf4cpp::TimePoint{std::chrono::seconds{util::unpack_integral<int64_t>(inlined)}}, std::nullopt);
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dateTime>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return rdf4cpp::datatypes::registry::util::compare_time_points(lhs.first, lhs.second, rhs.first, rhs.second);
}
#endif

template struct LiteralDatatypeImpl<xsd_dateTime,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::datatypes::registry