#ifndef RDF4CPP_TIMEZONE_HPP
#define RDF4CPP_TIMEZONE_HPP

#include <chrono>
#include <format>
#include <string_view>

#include <dice/hash.hpp>

#include <rdf4cpp/datatypes/rdf.hpp>
#include <rdf4cpp/datatypes/registry/util/CharConvExt.hpp>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp {
struct Timezone {
    // heavily inspired by https://howardhinnant.github.io/date/tz.html#Examples

    std::chrono::minutes offset = std::chrono::minutes{0};

    static constexpr const char *begin_tokens = "Z+-";

    constexpr Timezone() = default;

    inline explicit Timezone(const std::chrono::time_zone *tz, std::chrono::time_point<std::chrono::system_clock> n = std::chrono::system_clock::now())
        : offset(std::chrono::duration_cast<std::chrono::minutes>(tz->get_info(n).offset)) {
    }

    constexpr explicit Timezone(std::chrono::hours h) noexcept
        : offset(h) {}

    constexpr explicit Timezone(std::chrono::minutes h) noexcept
        : offset(h) {}

    constexpr auto operator<=>(const Timezone &) const noexcept = default;

    static constexpr Timezone parse(std::string_view v, std::string_view dt) {
        Timezone tz{};
        if (v == "Z") {
            return tz;
        }
        bool negative = false;
        if (v[0] == '-') {
            negative = true;
        }
        v = v.substr(1);
        auto sep = v.find(':');
        if (sep == std::string::npos) {
            throw InvalidNode{std::format("{} parsing error: timezone expected :", dt)};
        }
        std::chrono::hours h{datatypes::registry::util::from_chars<int32_t, "timezone">(v.substr(0, sep))};
        tz.offset = std::chrono::minutes{datatypes::registry::util::from_chars<int32_t, "timezone">(v.substr(sep + 1))} + std::chrono::minutes{h};
        if (negative) {
            tz.offset *= -1;
        }
        if (tz.offset.count() < -840 || tz.offset.count() > 840) {
            throw InvalidNode{std::format("{} parsing error: timezone offset too big", dt)};
        }
        return tz;
    }

    static constexpr std::optional<Timezone> parse_optional(std::string_view &s, std::string_view dt) {
        auto p = s.find_first_of(begin_tokens, 1);
        if (p == 0 || p == std::string::npos)
            return std::nullopt;
        auto pre = s.substr(0, p);
        auto tz = parse(s.substr(p), dt);
        s = pre;
        return tz;
    }

    // sign, hours, :, minutes
    static constexpr size_t max_canonical_string_chars = 1+(std::numeric_limits<int64_t>::digits10+1)+1+2;
    template<std::output_iterator<char> T>
    T to_canonical_string(T o) const noexcept {
        if (offset == std::chrono::minutes{0}) {
            *o = 'Z';
            ++o;
            return o;
        }
        auto h = std::chrono::floor<std::chrono::hours>(std::chrono::abs(offset));
        auto m = std::chrono::abs(offset) - h;
        return std::format_to(o, "{}{:02}:{:02}", offset >= std::chrono::minutes{0} ? '+' : '-', h.count(), m.count());
    }
    [[nodiscard]] std::string to_canonical_string() const noexcept {
        std::string buf{};
        buf.reserve(max_canonical_string_chars);
        to_canonical_string(std::back_inserter(buf));
        return buf;
    }

    [[nodiscard]] const std::chrono::time_zone *get_tz(std::chrono::time_point<std::chrono::system_clock> n = std::chrono::system_clock::now()) const {
        for (const auto &tz : std::chrono::get_tzdb().zones) {
            if (tz.get_info(n).offset == std::chrono::seconds(offset)) {
                return &tz;
            }
        }
        return nullptr;
    }

    template<typename Duration>
    [[nodiscard]] auto to_sys(const std::chrono::local_time<Duration> &tp) const noexcept {
        return std::chrono::sys_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp - offset).time_since_epoch()};
    }

    template<typename Duration>
    [[nodiscard]] auto to_local(const std::chrono::sys_time<Duration> &tp) const noexcept {
        return std::chrono::local_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp + offset).time_since_epoch()};
    }

    template<typename Duration>
    [[nodiscard]] std::chrono::sys_info get_info(const std::chrono::sys_time<Duration> &) const noexcept {
        return std::chrono::sys_info{
                std::chrono::sys_seconds{std::chrono::seconds{0l}},
                std::chrono::sys_seconds{std::chrono::seconds{std::numeric_limits<int64_t>::max()}},
                offset,
                std::chrono::minutes{0},
                to_canonical_string()};
    }

    const Timezone *operator->() const noexcept {
        return this;
    }

    static constexpr Timezone max_value() noexcept {
        return Timezone{std::chrono::hours{14}};
    };
    static constexpr Timezone min_value() noexcept {
        return Timezone{std::chrono::hours{-14}};
    };
};

using OptionalTimezone = std::optional<Timezone>;

using TimePoint = std::chrono::time_point<std::chrono::local_t, std::chrono::milliseconds>;
// system_clock does not use leap seconds, as required by rdf (xsd)
using TimePointSys = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
using ZonedTime = std::chrono::zoned_time<std::chrono::milliseconds, Timezone>;

template<typename I>
struct Year {
    // adapted from https://howardhinnant.github.io/date_algorithms.html
    I year;

    [[nodiscard]] constexpr bool is_leap() const noexcept(noexcept(year % 100)) {
        return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    }

    constexpr auto operator<=>(Year const &) const noexcept = default;
};

template<typename Y>
struct Date {
    // adapted from https://howardhinnant.github.io/date_algorithms.html
    Year<Y> year;
    std::chrono::month month;
    std::chrono::day day;

    using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::duration<Y, std::chrono::days::period>>;

private:
    static constexpr bool arithmetic_noexcept = noexcept(Y{} + Y{}) && noexcept(Y{} - Y{}) && noexcept(Y{} / Y{}) && noexcept(Y{} * Y{});

    static constexpr std::chrono::day last_day_in_month(Year<Y> year, std::chrono::month month) noexcept(noexcept(year.is_leap())) {
        assert(month.ok());
        constexpr unsigned char common[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        auto m = static_cast<unsigned int>(month);
        return std::chrono::day{m != 2 || !year.is_leap() ? common[m - 1] : 29u};
    }

public:
    constexpr explicit Date(std::chrono::year_month_day ymd) noexcept(noexcept(Year<Y>(static_cast<int>(ymd.year()))))
        : year(static_cast<int>(ymd.year())), month(ymd.month()), day(ymd.day()) {
    }
    constexpr Date(Year<Y> y, std::chrono::month m, std::chrono::day d) noexcept
        : year(y), month(m), day(d) {
    }
    constexpr Date(Year<Y> y, std::chrono::month m, std::chrono::last_spec) noexcept(noexcept(last_day_in_month(y, m)))
        : year(y), month(m), day(last_day_in_month(y, m)) {
    }
    constexpr explicit Date(time_point sd) noexcept(arithmetic_noexcept) {
        static_assert(std::numeric_limits<unsigned>::digits >= 18, "This algorithm has not been ported to a 16 bit unsigned integer");
        static_assert(std::numeric_limits<Y>::digits >= 20, "This algorithm has not been ported to a 16 bit signed integer");
        Y z = sd.time_since_epoch().count();
        z += 719468;
        Y const era = (z >= 0 ? z : z - 146096) / 146097;
        auto const doe = static_cast<unsigned>(z - era * 146097);                    // [0, 146096]
        unsigned const yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;  // [0, 399]
        Y const y = static_cast<Y>(yoe) + era * 400;
        unsigned const doy = doe - (365 * yoe + yoe / 4 - yoe / 100);  // [0, 365]
        unsigned const mp = (5 * doy + 2) / 153;                       // [0, 11]
        unsigned const d = doy - (153 * mp + 2) / 5 + 1;               // [1, 31]
        unsigned const m = mp < 10 ? mp + 3 : mp - 9;                  // [1, 12]
        year = Year<Y>{y + (m <= 2)};
        month = std::chrono::month{m};
        day = std::chrono::day{d};
    }

    [[nodiscard]] constexpr time_point to_time_point() const noexcept(arithmetic_noexcept) {
        static_assert(std::numeric_limits<unsigned>::digits >= 18, "This algorithm has not been ported to a 16 bit unsigned integer");
        static_assert(std::numeric_limits<Y>::digits >= 20, "This algorithm has not been ported to a 16 bit signed integer");
        auto y = year.year;
        auto m = static_cast<unsigned int>(month);
        auto d = static_cast<unsigned int>(day);
        y -= m <= 2;
        auto const era = (y >= 0 ? y : y - 399) / 400;
        auto const yoe = static_cast<unsigned>(y - era * 400);                 // [0, 399]
        unsigned const doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;  // [0, 365]
        unsigned const doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;            // [0, 146096]
        // note that the epoch of system_clock is specified as 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970
        return time_point{typename time_point::duration{era * 146097 + static_cast<Y>(doe) - 719468}};
    }

    [[nodiscard]] constexpr bool ok() const noexcept(noexcept(last_day_in_month(year, month))) {
        return month.ok() && day.ok() && day <= last_day_in_month(year, month);
    }

    constexpr auto operator<=>(Date const &) const noexcept = default;
};

namespace util {

using duration_backend = boost::multiprecision::checked_int128_t;
constexpr auto i = sizeof(__int128);

inline constexpr std::chrono::year_month_day time_point_replacement_date = std::chrono::year(1972) / std::chrono::December / std::chrono::last;
inline constexpr std::chrono::milliseconds time_point_replacement_time_of_day{0};

constexpr TimePoint construct_timepoint(std::chrono::year_month_day date, std::chrono::milliseconds time_of_day) noexcept {
    auto sd = static_cast<std::chrono::local_days>(date);
    auto ms = static_cast<TimePoint>(sd);
    ms += time_of_day;
    return ms;
}

} // namespace util

}  // namespace rdf4cpp

namespace std::chrono {
    template<>
    struct zoned_traits<::rdf4cpp::Timezone> {
        static ::rdf4cpp::Timezone default_zone() noexcept {
            return ::rdf4cpp::Timezone{};
        }
    };
} // namespace std::chrono

#ifndef DOXYGEN_PARSER
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::Timezone> {
    static size_t dice_hash(rdf4cpp::Timezone const &x) noexcept {
        auto off = x.offset.count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(off);
    }
};
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::OptionalTimezone> {
    static size_t dice_hash(rdf4cpp::OptionalTimezone const &x) noexcept {
        auto off = x.has_value() ? x->offset.count() : std::chrono::minutes{std::chrono::hours{15}}.count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(off);
    }
};
#endif

#endif  //RDF4CPP_TIMEZONE_HPP
