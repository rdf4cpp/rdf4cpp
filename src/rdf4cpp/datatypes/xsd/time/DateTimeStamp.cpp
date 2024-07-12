#include "DateTimeStamp.hpp"

#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_dateTimeStamp>::cpp_type capabilities::Default<xsd_dateTimeStamp>::from_string(std::string_view s) {
    using namespace datatypes::registry::util;
    auto year = parse_date_time_fragment<std::chrono::year, int, '-', identifier>(s);
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-', identifier>(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, 'T', identifier>(s);
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':', identifier>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':', identifier>(s);
    auto p = s.find_first_of(rdf4cpp::Timezone::begin_tokens);
    if (p == 0)
        throw InvalidNode{std::format("{} parsing error: invalid seconds", identifier)};
    if (p == std::string::npos)
        throw InvalidNode{std::format("{} parsing error: missing timezone", identifier)};
    auto tz = rdf4cpp::Timezone::parse(s.substr(p), identifier);
    std::chrono::milliseconds ms = parse_milliseconds<identifier>(s.substr(0, p));
    auto date = year / month / day;
    if (registry::relaxed_parsing_mode && !date.ok())
        date = normalize(date);
    if (!date.ok()) {
        throw InvalidNode(std::format("{} parsing error: {:%Y-%m-%d} is invalid", identifier, date));
    }
    if (!registry::relaxed_parsing_mode) {
        if (minutes < std::chrono::minutes(0) || minutes > std::chrono::hours(1))
            throw InvalidNode(std::format("{} parsing error: minutes out of range", identifier));
        if (hours < std::chrono::hours(0) || hours > std::chrono::days(1))
            throw InvalidNode(std::format("{} parsing error: hours out of range", identifier));
        if (ms < std::chrono::seconds(0) || ms > std::chrono::minutes(1))
            throw InvalidNode(std::format("{} parsing error: seconds out of range", identifier));
    }
    auto time = hours + minutes + ms;
    if (!registry::relaxed_parsing_mode) {
        if (time == std::chrono::hours{24}) {
            date = std::chrono::year_month_day{std::chrono::local_days{date} + std::chrono::days{1}};
            if (!date.ok())
                throw InvalidNode(std::format("{} parsing error: {:%Y-%m-%d} is invalid", identifier, date));
            time = std::chrono::hours{0};
        } else if (time > std::chrono::hours{24}) {
            throw InvalidNode(std::format("{} parsing error: invalid time of day", identifier));
        }
    }

    return rdf4cpp::ZonedTime{tz, rdf4cpp::util::construct_timepoint(date, time)};
}

template<>
bool capabilities::Default<xsd_dateTimeStamp>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    //year,-,month,-,day, T, hours,:,min,:,sec, tz
    std::array<char, registry::util::chrono_max_canonical_string_chars::year + 1 +
                             registry::util::chrono_max_canonical_string_chars::month + 1 +
                             registry::util::chrono_max_canonical_string_chars::day + 1 +
                             registry::util::chrono_max_canonical_string_chars::hours + 1 +
                             registry::util::chrono_max_canonical_string_chars::minutes + 1 +
                             registry::util::chrono_max_canonical_string_chars::seconds + Timezone::max_canonical_string_chars>
            buff;
    char *it = std::format_to(buff.data(), "{:%Y-%m-%dT%H:%M:%S}", value);
    it = util::canonical_seconds_remove_empty_millis(it);
    it = value.get_time_zone().to_canonical_string(it);
    size_t const len = it - buff.data();
    assert(len <= buff.size());
    return writer::write_str(std::string_view(buff.data(), len), writer);
}

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_dateTimeStamp>::try_into_inlined(cpp_type const &value) noexcept {
    if (value.get_time_zone() != xsd::DateTimeStamp::inlining_default_timezone)
        return std::nullopt;
    auto tp_sec = std::chrono::floor<std::chrono::seconds>(value.get_sys_time());
    if ((value.get_sys_time() - tp_sec).count() != 0)
        return std::nullopt;
    auto s = static_cast<int64_t>(tp_sec.time_since_epoch().count());
    return util::try_pack_integral<storage::identifier::LiteralID>(s);
}

template<>
capabilities::Inlineable<xsd_dateTimeStamp>::cpp_type capabilities::Inlineable<xsd_dateTimeStamp>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    return rdf4cpp::ZonedTime{xsd::DateTimeStamp::inlining_default_timezone, rdf4cpp::TimePointSys{std::chrono::seconds{util::unpack_integral<int64_t>(inlined)}}};
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
    return rdf4cpp::ZonedTime{*value.second, value.first};
}
#endif

template struct LiteralDatatypeImpl<xsd_dateTimeStamp,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Subtype,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::datatypes::registry