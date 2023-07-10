#include <rdf4cpp/rdf/datatypes/xsd/time/Duration.hpp>

#include <ranges>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_duration>::cpp_type capabilities::Default<xsd_duration>::from_string(std::string_view s) {
    bool negative = false;
    if (!s.empty() && s[0] == '-') {
        negative = true;
        s = s.substr(1);
    }
    if (s.empty() || s[0] != 'P')
        throw std::invalid_argument{"duration missing P"};
    s = s.substr(1);
    auto p = s.find('T');
    auto date = s.substr(0, p);
    auto time = p == std::string::npos ? std::string_view{""} : s.substr(p);
    auto years = parse_duration_fragment<std::chrono::years, uint64_t, 'Y'>(date);
    auto months = parse_duration_fragment<std::chrono::months, uint64_t, 'M'>(date);
    auto days = parse_duration_fragment<std::chrono::days, uint64_t, 'D'>(date);
    if (!time.empty()) {
        if (time[0] != 'T')
            throw std::invalid_argument{"duration missing T"};
        time = time.substr(1);
    }
    auto hours = parse_duration_fragment<std::chrono::hours, uint64_t, 'H'>(time);
    auto minutes = parse_duration_fragment<std::chrono::minutes, uint64_t, 'M'>(time);
    auto seconds = parse_duration_milliseconds(time);

    std::chrono::months m{};
    if (years.has_value())
        m += *years;
    if (months.has_value())
        m += *months;
    std::chrono::milliseconds ms{};
    if (days.has_value())
        ms += *days;
    if (!date.empty() || !time.empty())
        throw std::invalid_argument{"expected end of string"};
    if (!years.has_value() && !months.has_value() && !days.has_value() && !hours.has_value() && !minutes.has_value() && !seconds.has_value()) {
        throw std::invalid_argument{"duration without any fields"};
    }
    if (hours.has_value())
        ms += *hours;
    if (minutes.has_value())
        ms += *minutes;
    if (seconds.has_value())
        ms += *seconds;
    if (negative) {
        m = -m;
        ms = -ms;
    }
    return std::make_pair(m, ms);
}

template<>
std::string capabilities::Default<xsd_duration>::to_canonical_string(const cpp_type &value) noexcept {
    if (value.first.count() == 0 && value.second.count() == 0)
        return "PT0.000S";
    std::stringstream str{};
    std::chrono::months m_rem = value.first;
    std::chrono::milliseconds ms_rem = value.second;
    if (m_rem.count() < 0) {
        str << '-';
        m_rem = -m_rem;
        ms_rem = -ms_rem;
    }
    str << 'P';
    auto years = std::chrono::floor<std::chrono::years>(m_rem);
    if (years.count() != 0)
        str << years.count() << 'Y';
    m_rem -= years;
    if (m_rem.count() != 0)
        str << m_rem.count() << 'M';
    auto days = std::chrono::floor<std::chrono::days>(ms_rem);
    if (days.count() != 0)
        str << days.count() << 'D';
    ms_rem -= days;
    if (ms_rem.count() != 0) {
        str << 'T';
        auto hours = std::chrono::floor<std::chrono::hours>(ms_rem);
        if (hours.count() != 0)
            str << hours.count() << 'H';
        ms_rem -= hours;
        auto minutes = std::chrono::floor<std::chrono::minutes>(ms_rem);
        if (minutes.count() != 0)
            str << minutes.count() << 'M';
        ms_rem -= minutes;
        if (ms_rem.count() != 0)
            str << std::format("{:%S}S", ms_rem);
    }

    return str.str();
}

template<>
std::partial_ordering capabilities::Comparable<xsd_duration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    static constexpr std::array<TimePoint, 4> to_compare{
            construct(std::chrono::year{1696} / 9 / 1, std::chrono::milliseconds{0}),
            construct(std::chrono::year{1697} / 2 / 1, std::chrono::milliseconds{0}),
            construct(std::chrono::year{1903} / 3 / 1, std::chrono::milliseconds{0}),
            construct(std::chrono::year{1903} / 7 / 1, std::chrono::milliseconds{0}),
    };
    auto cmp = [lhs, rhs](TimePoint tp) {
        return TimeComparer<TimePoint>::compare(add_duration_to_date_time(tp, lhs), std::nullopt, add_duration_to_date_time(tp, rhs), std::nullopt);
    };
    std::partial_ordering o = cmp(to_compare[0]);
    for (unsigned int i = 1; i < to_compare.size(); ++i) {
        if (o != cmp(to_compare[i]))
            return std::partial_ordering::unordered;
    }
    return o;
}

template struct LiteralDatatypeImpl<xsd_duration,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry