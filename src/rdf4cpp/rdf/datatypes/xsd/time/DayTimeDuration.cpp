#include <rdf4cpp/rdf/datatypes/xsd/time/DayTimeDuration.hpp>

#include <ranges>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_dayTimeDuration>::cpp_type capabilities::Default<xsd_dayTimeDuration>::from_string(std::string_view s) {
    bool negative = false;
    if (s[0] == '-') {
        negative = true;
        s = s.substr(1);
    }
    if (s[0] != 'P')
        throw std::invalid_argument{"duration missing P"};
    s = s.substr(1);
    auto years = parse_duration_fragment<std::chrono::years, uint64_t, 'Y'>(s);
    auto months = parse_duration_fragment<std::chrono::months, uint64_t, 'M'>(s);
    auto days = parse_duration_fragment<std::chrono::days, uint64_t, 'D'>(s);
    if (years.has_value())
        throw std::invalid_argument{"DayTimeDuration with years"};
    if (months.has_value())
        throw std::invalid_argument{"DayTimeDuration with months"};
    std::chrono::milliseconds ms{};
    if (days.has_value())
        ms += *days;
    if (s.empty()) {
        if (!days.has_value()) {
            throw std::invalid_argument{"duration without any fields"};
        }
        if (negative) {
            ms = -ms;
        }
        return ms;
    }
    if (s[0] != 'T')
        throw std::invalid_argument{"duration missing T"};
    s = s.substr(1);
    auto hours = parse_duration_fragment<std::chrono::hours, uint64_t, 'H'>(s);
    auto minutes = parse_duration_fragment<std::chrono::minutes, uint64_t, 'M'>(s);
    auto seconds = parse_duration_fragment<std::chrono::seconds, uint64_t, 'S'>(s);
    if (!s.empty())
        throw std::invalid_argument{"expected end of string"};
    if (hours.has_value())
        ms += *hours;
    if (minutes.has_value())
        ms += *minutes;
    if (seconds.has_value())
        ms += *seconds;
    if (negative) {
        ms = -ms;
    }
    return ms;
}

template<>
std::string capabilities::Default<xsd_dayTimeDuration>::to_canonical_string(const cpp_type &value) noexcept {
    if (value.count() == 0)
        return "PT0S";
    std::stringstream str{};
    std::chrono::milliseconds ms_rem = value;
    if (ms_rem.count() < 0) {
        str << '-';
        ms_rem = -ms_rem;
    }
    str << 'P';
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
        auto seconds = std::chrono::floor<std::chrono::seconds>(ms_rem);
        if (seconds.count() != 0)
            str << seconds.count() << 'S';
    }

    return str.str();
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dayTimeDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs <=> rhs;
}

template struct LiteralDatatypeImpl<xsd_dayTimeDuration,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry