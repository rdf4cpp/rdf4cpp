#ifndef RDF4CPP_TIMEZONE_HPP
#define RDF4CPP_TIMEZONE_HPP

#include <chrono>
#include <format>
#include <string_view>

#include <dice/hash.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::util {
class Timezone {
    // heavily inspired by https://howardhinnant.github.io/date/tz.html#Examples
public:
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
        std::chrono::hours h{datatypes::registry::util::from_chars<int32_t>(v.substr(0, sep))};
        tz.offset = std::chrono::minutes{datatypes::registry::util::from_chars<int32_t>(v.substr(sep + 1))} + std::chrono::minutes{h};
        if (negative)
            tz.offset *= -1;
        if (tz.offset.count() < -840 || tz.offset.count() > 840)
            throw std::invalid_argument{"timezone offset too big"};
        return tz;
    }

    static constexpr std::optional<Timezone> parse_optional(std::string_view &s) {
        auto p = s.find_first_of(begin_tokens, 1);
        if (p == 0 || p == std::string::npos)
            return std::nullopt;
        auto pre = s.substr(0, p);
        auto tz = parse(s.substr(p));
        s = pre;
        return tz;
    }

    [[nodiscard]] std::string to_canonical_string() const noexcept {
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
    [[nodiscard]] auto to_sys(const std::chrono::local_time<Duration> &tp) const noexcept {
        return std::chrono::sys_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp - offset).time_since_epoch()};
    }

    template<class Duration>
    [[nodiscard]] auto to_local(const std::chrono::sys_time<Duration> &tp) const noexcept {
        return std::chrono::local_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp + offset).time_since_epoch()};
    }

    template<class Duration>
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

constexpr std::chrono::year_month_day TimePointReplacementDate = std::chrono::year(1972) / std::chrono::December / std::chrono::last;
constexpr std::chrono::milliseconds TimePointReplacementTimeOfDay{0};

constexpr rdf::util::TimePoint construct(std::chrono::year_month_day date, std::chrono::milliseconds time_of_day) noexcept {
    auto sd = static_cast<std::chrono::local_days>(date);
    auto ms = static_cast<rdf::util::TimePoint>(sd);
    ms += time_of_day;
    return ms;
}

}  // namespace rdf4cpp::rdf::util

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::util::Timezone> {
    static size_t dice_hash(rdf4cpp::rdf::util::Timezone const &x) noexcept {
        auto off = x.offset.count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(off);
    }
};
template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::util::OptionalTimezone> {
    static size_t dice_hash(rdf4cpp::rdf::util::OptionalTimezone const &x) noexcept {
        auto off = x.has_value() ? x->offset.count() : std::chrono::minutes{std::chrono::hours{15}}.count();
        return dice::hash::dice_hash_templates<Policy>::dice_hash(off);
    }
};

#endif  //RDF4CPP_TIMEZONE_HPP
