#ifndef RDF4CPP_DATETIMEUTILS_HPP
#define RDF4CPP_DATETIMEUTILS_HPP

#include <chrono>
#include <format>
#include <string_view>

#include <dice/hash.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#endif  //RDF4CPP_DATETIMEUTILS_HPP

namespace rdf4cpp::rdf::datatypes::registry::util {
template<class ResultType, class ParsingType, char Separator>
ResultType parse_date_time_fragment(std::string_view &s) {
    std::string_view res_s = s;
    if constexpr (Separator != '\0') {
        auto p = s.find(Separator, 1);
        if (p == std::string::npos)
            throw std::invalid_argument(std::format("missing {}", Separator));
        res_s = s.substr(0, p);
        s = s.substr(p + 1);
    }
    return ResultType{from_chars<ParsingType>(res_s)};
}

template<class ResultType, class ParsingType, char Separator>
std::optional<ResultType> parse_duration_fragment(std::string_view &s) {
    if (s.empty())
        return std::nullopt;
    std::string_view res_s = s;
    auto p = s.find(Separator);
    if (p == std::string::npos)
        return std::nullopt;
    res_s = s.substr(0, p);
    s = s.substr(p + 1);
    return ResultType{from_chars<ParsingType>(res_s)};
}

inline std::chrono::milliseconds parse_milliseconds(std::string_view s) {
    auto p = s.find('.');
    std::chrono::milliseconds ms{};
    if (p != std::string::npos) {
        auto milli_s = s.substr(p + 1, 3);
        ms = std::chrono::milliseconds{from_chars<unsigned int>(milli_s)};
        for (size_t i = milli_s.length(); i < 3; ++i) {
            ms *= 10;
        }
        s = s.substr(0, p);
    }
    std::chrono::seconds sec{from_chars<unsigned int>(s)};
    return sec + ms;
}
inline std::optional<std::chrono::milliseconds> parse_duration_milliseconds(std::string_view &s) {
    if (s.empty())
        return std::nullopt;
    std::string_view res_s = s;
    auto p = s.find('S');
    if (p == std::string::npos)
        return std::nullopt;
    res_s = s.substr(0, p);
    s = s.substr(p + 1);
    return parse_milliseconds(res_s);
}

inline rdf::util::TimePoint add_duration_to_date_time(rdf::util::TimePoint tp, std::pair<std::chrono::months, std::chrono::milliseconds> d) {
    auto days = std::chrono::floor<std::chrono::days>(tp);
    auto time = tp - days;
    std::chrono::year_month_day ymd{days};
    ymd += d.first;
    if (!ymd.ok())
        ymd = ymd.year() / ymd.month() / std::chrono::last;
    return rdf::util::construct(ymd, time + d.second);
}

class TimeComparer {
    static inline rdf::util::TimePointSys apply_timezone(rdf::util::TimePoint t, rdf::util::Timezone tz) {
        return rdf::util::ZonedTime{tz, t}.get_sys_time();
    }

public:
    static inline std::partial_ordering compare(rdf::util::TimePoint a, std::optional<rdf::util::Timezone> atz, rdf::util::TimePoint b, std::optional<rdf::util::Timezone> btz) {
        rdf::util::TimePoint a_tp = a;
        rdf::util::TimePoint b_tp = b;
        if (atz.has_value()) {
            rdf::util::TimePointSys a_sys = apply_timezone(a_tp, *atz);
            if (btz.has_value()) {
                return a_sys <=> apply_timezone(b_tp, *btz);
            } else {
                auto p14 = a_sys <=> apply_timezone(b_tp, rdf::util::Timezone::max_value());
                auto m14 = a_sys <=> apply_timezone(b_tp, rdf::util::Timezone::min_value());
                if (p14 != m14)
                    return std::partial_ordering::unordered;
                return p14;
            }
        } else {
            if (btz.has_value()) {
                rdf::util::TimePointSys b_sys = apply_timezone(b_tp, *btz);
                auto p14 = apply_timezone(a_tp, rdf::util::Timezone::max_value()) <=> b_sys;
                auto m14 = apply_timezone(a_tp, rdf::util::Timezone::min_value()) <=> b_sys;
                if (p14 != m14)
                    return std::partial_ordering::unordered;
                return p14;
            } else {
                return a_tp <=> b_tp;
            }
        }
    }
};
template<std::unsigned_integral T>
constexpr T numberOfBits(T x) {
    return x < 2 ? x : 1 + numberOfBits(x >> 1);
}
template<class TimeType>
    requires(sizeof(TimeType) <= 2)
struct __attribute__((__packed__)) InliningHelper {
    uint16_t tz_offset;
    TimeType time_value;

    static constexpr int tz_shift = rdf::util::Timezone::max_value().offset.count() + 1;
    static_assert(numberOfBits(static_cast<unsigned int>(rdf::util::Timezone::max_value().offset.count() + tz_shift)) == 11);

    static constexpr uint16_t encode_tz(rdf::util::OptionalTimezone tz) noexcept {
        if (tz.has_value())
            return static_cast<uint16_t>(tz->offset.count() + tz_shift);
        else
            return 0;
    }

    constexpr InliningHelper(TimeType t, rdf::util::OptionalTimezone tz) noexcept : tz_offset(encode_tz(tz)), time_value(t) {
    }

    [[nodiscard]] constexpr rdf::util::OptionalTimezone decode_tz() const noexcept {
        if (tz_offset == 0)
            return std::nullopt;
        else
            return rdf::util::Timezone{std::chrono::minutes{static_cast<int>(tz_offset) - tz_shift}};
    }
};
struct __attribute__((__packed__)) InliningHelperPacked {
    static constexpr std::size_t width = storage::node::identifier::LiteralID::width;
    static constexpr std::size_t tv_width = width - 11;

    uint16_t tz_offset : 11;
    uint32_t time_value : tv_width;

private:
    [[maybe_unused]] uint32_t padding : 64 - width = 0;  // to make sure the rest of the int64 is 0

public:
    static constexpr int tz_shift = rdf::util::Timezone::max_value().offset.count() + 1;
    static_assert(numberOfBits(static_cast<unsigned int>(rdf::util::Timezone::max_value().offset.count() + tz_shift)) == 11);

    static constexpr uint16_t encode_tz(rdf::util::OptionalTimezone tz) noexcept {
        if (tz.has_value())
            return static_cast<uint16_t>(tz->offset.count() + tz_shift);
        else
            return 0;
    }

    constexpr InliningHelperPacked(uint32_t t, rdf::util::OptionalTimezone tz) noexcept : tz_offset(encode_tz(tz)), time_value(t) {
    }

    [[nodiscard]] rdf::util::OptionalTimezone decode_tz() const noexcept {
        if (tz_offset == 0)
            return std::nullopt;
        else
            return rdf::util::Timezone{std::chrono::minutes{static_cast<int>(tz_offset) - tz_shift}};
    }
};

}  // namespace rdf4cpp::rdf::datatypes::registry::util
