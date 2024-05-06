#include "YearMonthDuration.hpp"

#include <ranges>
#include <rdf4cpp/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_yearMonthDuration>::cpp_type capabilities::Default<xsd_yearMonthDuration>::from_string(std::string_view s) {
    using namespace registry::util;
    bool negative = false;
    if (s[0] == '-') {
        negative = true;
        s = s.substr(1);
    }
    if (s[0] != 'P')
        throw std::runtime_error{"duration missing P"};
    s = s.substr(1);
    auto years = parse_duration_fragment<std::chrono::years, uint64_t, 'Y', identifier>(s);
    auto months = parse_duration_fragment<std::chrono::months, uint64_t, 'M', identifier>(s);
    std::chrono::months m{};
    if (years.has_value())
        m += *years;
    if (months.has_value())
        m += *months;
    if (!s.empty())
        throw std::runtime_error{"expected end of string"};
    if (!years.has_value() && !months.has_value()) {
        throw std::runtime_error{"duration without any fields"};
    }
    if (negative) {
        m = -m;
    }
    return m;
}

template<>
bool capabilities::Default<xsd_yearMonthDuration>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    if (value.count() == 0) {
        return writer::write_str("P0M", writer);
    }
    //-,P,years,months,
    std::array<char, 1+1+(std::numeric_limits<int64_t>::digits10+2)+3> buff;
    char* it = buff.data();
    std::chrono::months m_rem = value;
    if (m_rem.count() < 0) {
        *(it++) = '-';
        m_rem = -m_rem;
    }
    *(it++) = 'P';
    auto years = std::chrono::floor<std::chrono::years>(m_rem);
    if (years.count() != 0) {
        it = std::format_to(it, "{}", years.count());
        *(it++) = 'Y';
    }
    m_rem -= years;
    if (m_rem.count() != 0) {
        it = std::format_to(it, "{}", m_rem.count());
        *(it++) = 'M';
    }
    size_t const len = it - buff.data();
    assert(len <= buff.size());
    return writer::write_str(std::string_view(buff.data(), len), writer);
}

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_yearMonthDuration>::try_into_inlined(cpp_type const &value) noexcept {
    int64_t v = value.count();
    return util::try_pack_integral<storage::identifier::LiteralID>(v);
}

template<>
capabilities::Inlineable<xsd_yearMonthDuration>::cpp_type capabilities::Inlineable<xsd_yearMonthDuration>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    return std::chrono::months{util::unpack_integral<int64_t>(inlined)};
}

template<>
std::partial_ordering capabilities::Comparable<xsd_yearMonthDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs <=> rhs;
}

template<>
template<>
capabilities::Subtype<xsd_yearMonthDuration>::super_cpp_type<0> capabilities::Subtype<xsd_yearMonthDuration>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(value, std::chrono::milliseconds{0});
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_yearMonthDuration>::cpp_type, DynamicError> capabilities::Subtype<xsd_yearMonthDuration>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    if (value.first.count() == 0 && value.second.count() == 0)
        return value.first;
    if (value.first.count() != 0)
        return value.first;
    return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
}
#endif

template struct LiteralDatatypeImpl<xsd_yearMonthDuration,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;
}  // namespace rdf4cpp::datatypes::registry