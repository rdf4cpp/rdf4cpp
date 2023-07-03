#include <rdf4cpp/rdf/datatypes/xsd/time/YearMonthDuration.hpp>

#include <ranges>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_yearMonthDuration>::cpp_type capabilities::Default<xsd_yearMonthDuration>::from_string(std::string_view s) {
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
    std::chrono::months m{};
    if (years.has_value())
        m += *years;
    if (months.has_value())
        m += *months;
    if (!years.has_value() && !months.has_value()) {
        throw std::invalid_argument{"duration without any fields"};
    }
    if (negative) {
        m = -m;
    }
    return m;
}

template<>
std::string capabilities::Default<xsd_yearMonthDuration>::to_canonical_string(const cpp_type &value) noexcept {
    if (value.count() == 0 )
        return "P0M";
    std::stringstream str{};
    std::chrono::months m_rem = value;
    if (m_rem.count() < 0) {
        str << '-';
        m_rem = -m_rem;
    }
    str << 'P';
    auto years = std::chrono::floor<std::chrono::years>(m_rem);
    if (years.count() != 0)
        str << years.count() << 'Y';
    m_rem -= years;
    if (m_rem.count() != 0)
        str << m_rem.count() << 'M';

    return str.str();
}

template<>
std::partial_ordering capabilities::Comparable<xsd_yearMonthDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs <=> rhs;
}

template struct LiteralDatatypeImpl<xsd_yearMonthDuration,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry