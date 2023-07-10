#ifndef RDF4CPP_TIMEZONE_HPP
#define RDF4CPP_TIMEZONE_HPP

#include <chrono>
#include <format>
#include <string_view>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {
class Timezone {
    // heavily inspired by https://howardhinnant.github.io/date/tz.html#Examples
public:
    std::chrono::minutes offset = std::chrono::minutes{0};

    static constexpr const char *begin_tokens = "Z+-";

    constexpr Timezone() = default;

    constexpr explicit Timezone(const std::chrono::time_zone *tz, std::chrono::time_point<std::chrono::system_clock> n = std::chrono::system_clock::now())
        : offset(std::chrono::duration_cast<std::chrono::minutes>(tz->get_info(n).offset)) {
    }

    constexpr explicit Timezone(std::chrono::hours h)
        : offset(h) {}

    constexpr explicit Timezone(std::chrono::minutes h)
        : offset(h) {}

    constexpr auto operator<=>(const Timezone&) const noexcept = default;

    static constexpr Timezone parse(std::string_view v) {
        Timezone tz{};
        if (v == "Z")
            return tz;
        bool negative = false;
        if (v[0] == '-') {
            negative = true;
        }
        v = v.substr(1);
        auto sep = v.find(':');
        if (sep == std::string::npos)
            throw std::invalid_argument{"timezone missing :"};
        std::chrono::hours h{util::from_chars<int32_t>(v.substr(0, sep))};
        tz.offset = std::chrono::minutes{util::from_chars<int32_t>(v.substr(sep + 1))} + std::chrono::minutes{h};
        if (negative)
            tz.offset *= -1;
        if (tz.offset.count() < -840 || tz.offset.count() > 840)
            throw std::invalid_argument{"timezone offset too big"};
        return tz;
    }

    static constexpr std::pair<std::optional<Timezone>, std::string_view> try_parse(std::string_view s) {
        auto p = s.find_first_of(begin_tokens, 1);
        if (p == 0 || p == std::string::npos)
            return std::pair<std::optional<Timezone>, std::string_view>{std::nullopt, s};
        auto pre = s.substr(0, p);
        auto tz = parse(s.substr(p));
        return std::pair<std::optional<Timezone>, std::string_view>{tz, pre};
    }

    [[nodiscard]] std::string to_canonical_string() const {
        if (offset.count() == 0)
            return "Z";
        auto c = offset.count();
        return std::format("{:+}:{:02}", c / 60, std::abs(c) % 60);
    }

    [[nodiscard]] const std::chrono::time_zone *get_tz(std::chrono::time_point<std::chrono::system_clock> n = std::chrono::system_clock::now()) const {
        for (const auto &tz : std::chrono::get_tzdb().zones) {
            if (tz.get_info(n).offset == std::chrono::seconds(offset)) {
                return &tz;
            }
        }
        return nullptr;
    }

    template<class Duration>
    [[nodiscard]] auto to_sys(const std::chrono::local_time<Duration> &tp) const {
        return std::chrono::sys_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp - offset).time_since_epoch()};
    }

    template<class Duration>
    [[nodiscard]] auto to_local(const std::chrono::sys_time<Duration> &tp) const {
        return std::chrono::local_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp + offset).time_since_epoch()};
    }

    template<class Duration>
    [[nodiscard]] std::chrono::sys_info get_info(const std::chrono::sys_time<Duration> &) const {
        return std::chrono::sys_info{
                std::chrono::sys_seconds{std::chrono::seconds{0l}},
                std::chrono::sys_seconds{std::chrono::seconds{std::numeric_limits<int64_t>::max()}},
                offset,
                std::chrono::minutes{0},
                to_canonical_string()};
    }

    const Timezone* operator->() const {
        return this;
    }

    static constexpr Timezone max_value() {
        return Timezone{std::chrono::hours{14}};
    };
    static constexpr Timezone min_value() {
        return Timezone{std::chrono::hours{-14}};
    };
};

using OptionalTimezone = std::optional<Timezone>;

using TimePoint = std::chrono::time_point<std::chrono::local_t, std::chrono::milliseconds>;
// system_clock does not use leap seconds, as required by rdf (xsd)
using TimePointSys = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
using ZonedTime = std::chrono::zoned_time<std::chrono::milliseconds, Timezone>;

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
    return ResultType{util::from_chars<ParsingType>(res_s)};
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
    return ResultType{util::from_chars<ParsingType>(res_s)};
}

inline std::chrono::milliseconds parse_milliseconds(std::string_view s) {
    auto p = s.find('.');
    std::chrono::milliseconds ms{};
    if (p != std::string::npos) {
        auto milli_s = s.substr(p + 1, 3);
        ms = std::chrono::milliseconds{util::from_chars<unsigned int>(milli_s)};
        for (size_t i = milli_s.length(); i < 3; ++i) {
            ms *= 10;
        }
        s = s.substr(0, p);
    }
    std::chrono::seconds sec{util::from_chars<unsigned int>(s)};
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

constexpr std::chrono::year_month_day TimePointReplacementDate = std::chrono::year(1972) / std::chrono::December / std::chrono::last;
constexpr std::chrono::milliseconds TimePointReplacementTimeOfDay{0};
template<class T>
TimePoint to_point_on_timeline(T t) {
    return static_cast<TimePoint>(t);
}

constexpr TimePoint construct(std::chrono::year_month_day date, std::chrono::milliseconds time_of_day) {
    auto sd = static_cast<std::chrono::local_days>(date);
    auto ms = static_cast<TimePoint>(sd);
    ms += time_of_day;
    return ms;
}

inline TimePoint add_duration_to_date_time(TimePoint tp, std::pair<std::chrono::months, std::chrono::milliseconds> d) {
    auto days = std::chrono::floor<std::chrono::days>(tp);
    auto time = tp - days;
    std::chrono::year_month_day ymd{days};
    ymd += d.first;
    if (!ymd.ok())
        ymd = ymd.year() / ymd.month() / std::chrono::last;
    return construct(ymd, time + d.second);
}

template<class T>
class TimeComparer {
    static TimePointSys apply_timezone(TimePoint t, Timezone tz) {
        return ZonedTime{tz, t}.get_sys_time();
    }

public:
    static std::partial_ordering compare(T a, std::optional<Timezone> atz, T b, std::optional<Timezone> btz) {
        TimePoint a_tp = to_point_on_timeline(a);
        TimePoint b_tp = to_point_on_timeline(b);
        if (atz.has_value()) {
            TimePointSys a_sys = apply_timezone(a_tp, *atz);
            if (btz.has_value()) {
                return a_sys <=> apply_timezone(b_tp, *btz);
            } else {
                auto p14 = a_sys <=> apply_timezone(b_tp, Timezone::max_value());
                auto m14 = a_sys <=> apply_timezone(b_tp, Timezone::min_value());
                if (p14 != m14)
                    return std::partial_ordering::unordered;
                return p14;
            }
        } else {
            if (btz.has_value()) {
                TimePointSys b_sys = apply_timezone(b_tp, *btz);
                auto p14 = apply_timezone(a_tp, Timezone::max_value()) <=> b_sys;
                auto m14 = apply_timezone(a_tp, Timezone::min_value()) <=> b_sys;
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
struct InliningHelper {
    uint16_t tz_offset;
    TimeType time_value;

    static constexpr int tz_shift = Timezone::max_value().offset.count() + 1;
    static_assert(numberOfBits(static_cast<unsigned int>(Timezone::max_value().offset.count() + tz_shift)) == 11);

    static constexpr uint16_t encode_tz(OptionalTimezone tz) noexcept {
        if (tz.has_value())
            return static_cast<uint16_t>(tz->offset.count() + tz_shift);
        else
            return 0;
    }

    constexpr InliningHelper(TimeType t, OptionalTimezone tz) noexcept : tz_offset(encode_tz(tz)), time_value(t) {
    }

    [[nodiscard]] constexpr OptionalTimezone decode_tz() const noexcept {
        if (tz_offset == 0)
            return std::nullopt;
        else
            return Timezone{std::chrono::minutes{static_cast<int>(tz_offset) - tz_shift}};
    }
};
struct __attribute__((__packed__)) InliningHelperPacked {
    static constexpr std::size_t tv_width = storage::node::identifier::LiteralID::width-11;
    static constexpr std::size_t width = tv_width+11;

    uint16_t tz_offset:11;
    uint32_t time_value:tv_width;
    uint32_t padding:64-width = 0;

    static constexpr int tz_shift = Timezone::max_value().offset.count() + 1;
    static_assert(numberOfBits(static_cast<unsigned int>(Timezone::max_value().offset.count() + tz_shift)) == 11);

    static constexpr uint16_t encode_tz(OptionalTimezone tz) noexcept {
        if (tz.has_value())
            return static_cast<uint16_t>(tz->offset.count() + tz_shift);
        else
            return 0;
    }

    constexpr InliningHelperPacked(uint32_t t, OptionalTimezone tz) noexcept : tz_offset(encode_tz(tz)), time_value(t) {
    }

    [[nodiscard]] OptionalTimezone decode_tz() const noexcept {
        if (tz_offset == 0)
            return std::nullopt;
        else
            return Timezone{std::chrono::minutes{static_cast<int>(tz_offset) - tz_shift}};
    }
};

}  // namespace rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_TIMEZONE_HPP
