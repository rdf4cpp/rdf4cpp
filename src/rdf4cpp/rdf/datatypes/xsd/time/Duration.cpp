#include <rdf4cpp/rdf/datatypes/xsd/time/Duration.hpp>

#include <ranges>
#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_duration>::cpp_type capabilities::Default<xsd_duration>::from_string(std::string_view s) {
    using namespace registry::util;
    bool negative = false;
    if (!s.empty() && s[0] == '-') {
        negative = true;
        s = s.substr(1);
    }
    if (s.empty() || s[0] != 'P')
        throw std::runtime_error{"duration missing P"};
    s = s.substr(1);
    auto p = s.find('T');
    auto date = s.substr(0, p);
    auto time = p == std::string::npos ? std::string_view{""} : s.substr(p);
    auto years = parse_duration_fragment<std::chrono::years, uint64_t, 'Y', identifier>(date);
    auto months = parse_duration_fragment<std::chrono::months, uint64_t, 'M', identifier>(date);
    auto days = parse_duration_fragment<std::chrono::days, uint64_t, 'D', identifier>(date);
    if (!time.empty()) {
        if (time[0] != 'T')
            throw std::runtime_error{"duration missing T"};
        time = time.substr(1);
    }
    auto hours = parse_duration_fragment<std::chrono::hours, uint64_t, 'H', identifier>(time);
    auto minutes = parse_duration_fragment<std::chrono::minutes, uint64_t, 'M', identifier>(time);
    auto seconds = parse_duration_milliseconds<identifier>(time);

    std::chrono::months m{};
    if (years.has_value())
        m += *years;
    if (months.has_value())
        m += *months;
    std::chrono::milliseconds ms{};
    if (days.has_value())
        ms += *days;
    if (!date.empty() || !time.empty())
        throw std::runtime_error{"expected end of string"};
    if (!years.has_value() && !months.has_value() && !days.has_value() && !hours.has_value() && !minutes.has_value() && !seconds.has_value()) {
        throw std::runtime_error{"duration without any fields"};
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

struct __attribute__((__packed__)) InlinedDurationHelper {
    static constexpr std::size_t width = storage::node::identifier::LiteralID::width;
    static constexpr int seconds_width = 25;
    static constexpr int months_with = width - seconds_width - 1;

    static constexpr int64_t seconds_mask = ~int64_t{0} << seconds_width;
    static constexpr int64_t months_mask = ~int64_t{0} << months_with;

    unsigned int sign : 1;
    unsigned int seconds : seconds_width;
    unsigned int months : months_with;

    InlinedDurationHelper(unsigned int s, unsigned int sec, unsigned int mo) noexcept : sign(s), seconds(sec), months(mo) {
    }

private:
    [[maybe_unused]] unsigned int padding : 64 - width = 0;  // to make sure the rest of the int64 is 0
};

static_assert(registry::util::number_of_bits<unsigned int>((std::chrono::years{1} + std::chrono::seconds{0}).count()) == 25);

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_duration>::try_into_inlined(cpp_type const &value) noexcept {
    auto ms = value.second;
    auto months = value.first;
    unsigned int sign = 0;
    if (months < std::chrono::months{0}) {
        sign = 1;
        ms = -ms;
        months = -months;
    }
    auto sec = std::chrono::floor<std::chrono::seconds>(ms);
    if ((ms - sec) != std::chrono::milliseconds{0})
        return std::nullopt;
    if (sec.count() & InlinedDurationHelper::seconds_mask)
        return std::nullopt;
    if (months.count() & InlinedDurationHelper::months_mask)
        return std::nullopt;
    return util::pack<storage::node::identifier::LiteralID>(InlinedDurationHelper{
            sign, static_cast<unsigned int>(sec.count()), static_cast<unsigned int>(months.count())});
}

template<>
capabilities::Inlineable<xsd_duration>::cpp_type capabilities::Inlineable<xsd_duration>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    auto i = util::unpack<InlinedDurationHelper>(inlined);
    std::chrono::seconds sec{i.seconds};
    std::chrono::months months{i.months};
    if (i.sign == 1) {
        sec = -sec;
        months = -months;
    }
    return std::make_pair(months, std::chrono::milliseconds{sec});
}

template<>
std::partial_ordering capabilities::Comparable<xsd_duration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    static constexpr std::array<rdf::util::TimePoint, 4> to_compare{
            rdf::util::construct(std::chrono::year{1696} / 9 / 1, std::chrono::milliseconds{0}),
            rdf::util::construct(std::chrono::year{1697} / 2 / 1, std::chrono::milliseconds{0}),
            rdf::util::construct(std::chrono::year{1903} / 3 / 1, std::chrono::milliseconds{0}),
            rdf::util::construct(std::chrono::year{1903} / 7 / 1, std::chrono::milliseconds{0}),
    };
    auto cmp = [lhs, rhs](rdf::util::TimePoint tp) {
        auto l = registry::util::add_duration_to_date_time(tp, lhs);
        auto r = registry::util::add_duration_to_date_time(tp, rhs);
        if (l.time_since_epoch().count().is_invalid() || r.time_since_epoch().count().is_invalid())
            return std::partial_ordering::unordered;
        return registry::util::compare_time_points(registry::util::from_checked(l), std::nullopt, registry::util::from_checked(r), std::nullopt);
    };
    std::partial_ordering o = cmp(to_compare[0]);
    for (unsigned int i = 1; i < to_compare.size(); ++i) {
        if (o != cmp(to_compare[i]))
            return std::partial_ordering::unordered;
    }
    return o;
}
#endif

template struct LiteralDatatypeImpl<xsd_duration,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;
}  // namespace rdf4cpp::rdf::datatypes::registry