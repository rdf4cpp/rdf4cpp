#ifndef RDF4CPP_DATETIMEUTILS_HPP
#define RDF4CPP_DATETIMEUTILS_HPP

#include <chrono>
#include <format>
#include <string_view>

#include <dice/hash.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>
#include <rdf4cpp/rdf/util/CheckedInt.hpp>
#include <rdf4cpp/rdf/util/Timezone.hpp>

/**
 * @file
 * various date/time utilities
 * @note this header is not intended to be included by end users. But if something in here is useful for some edge cases, feel free to do so anyway.
 */

namespace rdf4cpp::rdf::datatypes::registry::util {
using CheckedMilliseconds = std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, std::milli>;
using CheckedMonths = std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, std::ratio<2629746>>;
static_assert(std::same_as<std::chrono::months::period, CheckedMonths::period>);
using CheckedTimePoint = std::chrono::time_point<std::chrono::local_t, CheckedMilliseconds>;
using CheckedZonedTime = std::chrono::zoned_time<CheckedMilliseconds, rdf4cpp::rdf::util::Timezone>;
using CheckedTimePointSys = std::chrono::time_point<std::chrono::system_clock, CheckedMilliseconds>;

/**
 * turns any duration to its CheckedIntegral counterpart.
 * @tparam R
 * @param v
 * @return
 */
template<class R>
std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, R> to_checked(std::chrono::duration<int64_t, R> v) noexcept {
    return std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, R>{v.count()};
}
/**
 * turns any CheckedIntegral duration back to its integer based duration.
 * @note undefined behavior, if v is invalid
 * @tparam R
 * @param v
 * @return
 */
template<class R>
std::chrono::duration<int64_t, R> from_checked(std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, R> v) noexcept {
    assert(!v.count().is_invalid());
    return std::chrono::duration<int64_t, R>{v.count().get_value()};
}
/**
 * turns any time_point to its CheckedIntegral counterpart.
 * @tparam C
 * @tparam R
 * @param v
 * @return
 */
template<class C, class R>
std::chrono::time_point<C, std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, R>> to_checked(std::chrono::time_point<C, std::chrono::duration<int64_t, R>> v) noexcept {
    return std::chrono::time_point<C, std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, R>>{to_checked(v.time_since_epoch())};
}
/**
 * turns any CheckedIntegral time_point back to its integer based time_point.
 * @note undefined behavior, if v is invalid
 * @tparam C
 * @tparam R
 * @param v
 * @return
 */
template<class C, class R>
std::chrono::time_point<C, std::chrono::duration<int64_t, R>> from_checked(std::chrono::time_point<C, std::chrono::duration<rdf4cpp::rdf::util::CheckedIntegral<int64_t>, R>> v) noexcept {
    return std::chrono::time_point<C, std::chrono::duration<int64_t, R>>{from_checked(v.time_since_epoch())};
}

/**
 * checks if a double fits into a specified integer type.
 * @tparam I
 * @param d
 * @return
 */
template<std::integral I>
bool fits_into(double d) {
    if (std::isnan(d) || std::isinf(d))
        return false;
    if (d >= static_cast<double>(std::numeric_limits<I>::max()))
        return false;
    if (d <= static_cast<double>(std::numeric_limits<I>::min()))
        return false;
    return true;
}

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

inline bool in_ymd_bounds(rdf4cpp::rdf::util::TimePoint tp) noexcept {
    static constexpr auto max = rdf4cpp::rdf::util::construct(std::chrono::year::max() / std::chrono::December / std::chrono::day{31},
                                                              std::chrono::days{1} - std::chrono::milliseconds{1});
    static constexpr auto min = rdf4cpp::rdf::util::construct(std::chrono::year::min() / std::chrono::January / std::chrono::day{1},
                                                              std::chrono::milliseconds{0});
    return max >= tp && tp >= min;
}

inline CheckedTimePoint add_duration_to_date_time(rdf4cpp::rdf::util::TimePoint tp, std::pair<std::chrono::months, std::chrono::milliseconds> d) noexcept {
    // only gets smaller, no overflow possible
    auto days = std::chrono::floor<std::chrono::days>(tp);
    auto time = tp - days;
    std::chrono::year_month_day ymd{days};

    int64_t m = static_cast<unsigned int>(ymd.month());
    m += static_cast<int>(ymd.year()) * 12; // it did fit into a 64 bit TimePoint before, so this cannot overflow

    if (__builtin_add_overflow(m, d.first.count(), &m))
        return CheckedTimePoint{CheckedMilliseconds{rdf4cpp::rdf::util::CheckedIntegral<int64_t>{0, true}}};
    int64_t y = (m-1) / 12;
    m = std::abs(m-1) % 12 + 1;
    if (y > static_cast<int>(std::chrono::year::max()) || y < static_cast<int>(std::chrono::year::min()))
        return CheckedTimePoint{CheckedMilliseconds{rdf4cpp::rdf::util::CheckedIntegral<int64_t>{0, true}}};

    ymd = std::chrono::year{static_cast<int>(y)} / std::chrono::month{static_cast<unsigned int>(m)} / ymd.day();
    if (!ymd.ok())
        ymd = ymd.year() / ymd.month() / std::chrono::last;

    CheckedTimePoint date = to_checked(static_cast<std::chrono::local_days>(ymd));
    date += to_checked(time);
    date += to_checked(d.second);

    return date;
}

static inline std::partial_ordering compare_time_points(rdf4cpp::rdf::util::TimePoint a, std::optional<rdf4cpp::rdf::util::Timezone> atz,
                                                        rdf4cpp::rdf::util::TimePoint b, std::optional<rdf4cpp::rdf::util::Timezone> btz) noexcept {
    auto apply_timezone = [](CheckedTimePoint t, rdf4cpp::rdf::util::Timezone tz) noexcept -> CheckedTimePointSys {
        return CheckedZonedTime{tz, t}.get_sys_time();
    };

    CheckedTimePoint a_tp = to_checked(a);
    CheckedTimePoint b_tp = to_checked(b);
    if (atz.has_value()) {
        CheckedTimePointSys a_sys = apply_timezone(a_tp, *atz);
        if (btz.has_value()) {
            return a_sys.time_since_epoch().count() <=> apply_timezone(b_tp, *btz).time_since_epoch().count();
        } else {
            auto p14 = a_sys.time_since_epoch().count() <=> apply_timezone(b_tp, rdf4cpp::rdf::util::Timezone::max_value()).time_since_epoch().count();
            auto m14 = a_sys.time_since_epoch().count() <=> apply_timezone(b_tp, rdf4cpp::rdf::util::Timezone::min_value()).time_since_epoch().count();
            if (p14 != m14)
                return std::partial_ordering::unordered;
            return p14;
        }
    } else {
        if (btz.has_value()) {
            CheckedTimePointSys b_sys = apply_timezone(b_tp, *btz);
            auto p14 = apply_timezone(a_tp, rdf4cpp::rdf::util::Timezone::max_value()).time_since_epoch().count() <=> b_sys.time_since_epoch().count();
            auto m14 = apply_timezone(a_tp, rdf4cpp::rdf::util::Timezone::min_value()).time_since_epoch().count() <=> b_sys.time_since_epoch().count();
            if (p14 != m14)
                return std::partial_ordering::unordered;
            return p14;
        } else {
            return a_tp.time_since_epoch().count() <=> b_tp.time_since_epoch().count();
        }
    }
}
template<std::unsigned_integral T>
constexpr T number_of_bits(T x) noexcept {
    return x < 2 ? x : 1 + number_of_bits(x >> 1);
}
template<class TimeType>
    requires(sizeof(TimeType) <= 2)
struct __attribute__((__packed__)) InliningHelper {
    uint16_t tz_offset;
    TimeType time_value;

    static constexpr int tz_shift = rdf4cpp::rdf::util::Timezone::max_value().offset.count() + 1;
    static_assert(number_of_bits(static_cast<unsigned int>(rdf4cpp::rdf::util::Timezone::max_value().offset.count() + tz_shift)) == 11);

    static constexpr uint16_t encode_tz(rdf4cpp::rdf::util::OptionalTimezone tz) noexcept {
        if (tz.has_value())
            return static_cast<uint16_t>(tz->offset.count() + tz_shift);
        else
            return 0;
    }

    constexpr InliningHelper(TimeType t, rdf4cpp::rdf::util::OptionalTimezone tz) noexcept : tz_offset(encode_tz(tz)), time_value(t) {
    }

    [[nodiscard]] constexpr rdf4cpp::rdf::util::OptionalTimezone decode_tz() const noexcept {
        if (tz_offset == 0)
            return std::nullopt;
        else
            return rdf4cpp::rdf::util::Timezone{std::chrono::minutes{static_cast<int>(tz_offset) - tz_shift}};
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
    static constexpr int tz_shift = rdf4cpp::rdf::util::Timezone::max_value().offset.count() + 1;
    static_assert(number_of_bits(static_cast<unsigned int>(rdf4cpp::rdf::util::Timezone::max_value().offset.count() + tz_shift)) == 11);

    static constexpr uint16_t encode_tz(rdf4cpp::rdf::util::OptionalTimezone tz) noexcept {
        if (tz.has_value())
            return static_cast<uint16_t>(tz->offset.count() + tz_shift);
        else
            return 0;
    }

    constexpr InliningHelperPacked(uint32_t t, rdf4cpp::rdf::util::OptionalTimezone tz) noexcept : tz_offset(encode_tz(tz)), time_value(t) {
    }

    [[nodiscard]] rdf4cpp::rdf::util::OptionalTimezone decode_tz() const noexcept {
        if (tz_offset == 0)
            return std::nullopt;
        else
            return rdf4cpp::rdf::util::Timezone{std::chrono::minutes{static_cast<int>(tz_offset) - tz_shift}};
    }
};

}  // namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_DATETIMEUTILS_HPP
