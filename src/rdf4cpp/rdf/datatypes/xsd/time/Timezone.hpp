#ifndef RDF4CPP_TIMEZONE_HPP
#define RDF4CPP_TIMEZONE_HPP

#include <chrono>
#include <format>
#include <string_view>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {
class Timezone {
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
        auto p = s.find_first_of(begin_tokens);
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
    auto to_sys(const std::chrono::local_time<Duration> &tp) const {
        return std::chrono::sys_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp - offset).time_since_epoch()};
    }

    template<class Duration>
    auto to_local(const std::chrono::sys_time<Duration> &tp) const {
        return std::chrono::local_time<std::common_type_t<Duration, std::chrono::seconds>>{(tp + offset).time_since_epoch()};
    }

    template<class Duration>
    std::chrono::sys_info get_info(const std::chrono::sys_time<Duration> &) const {
        return std::chrono::sys_info{
                std::chrono::sys_seconds{std::chrono::seconds{0l}},
                std::chrono::sys_seconds{std::chrono::seconds{std::numeric_limits<int64_t>::max()}},
                offset,
                std::chrono::minutes{0},
                to_canonical_string()};
    }
};

using OptionalTimezone = std::optional<Timezone>;

struct DateTime {
    std::chrono::year_month_day date;
    std::chrono::milliseconds time_of_day;

    constexpr auto operator<=>(const DateTime&) const noexcept = default;
};

template<class ResultType, class ParsingType, char Separator>
ResultType parse_date_time_fragment(std::string_view& s) {
    std::string_view res_s = s;
    if constexpr (Separator != '\0') {
        auto p = s.find(Separator);
        if (p == std::string::npos)
            throw std::invalid_argument(std::format("missing {}", Separator));
        res_s = s.substr(0, p);
        s = s.substr(p+1);
    }
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
}  // namespace rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_TIMEZONE_HPP
