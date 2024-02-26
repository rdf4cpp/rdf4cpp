#include "DayTimeDuration.hpp"

#include <ranges>
#include <rdf4cpp/rdf/datatypes/registry/util/DateTimeUtils.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_dayTimeDuration>::cpp_type capabilities::Default<xsd_dayTimeDuration>::from_string(std::string_view s) {
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
    auto days = parse_duration_fragment<std::chrono::days, uint64_t, 'D', identifier>(date);
    if (!time.empty()) {
        if (time[0] != 'T')
            throw std::runtime_error{"duration missing T"};
        time = time.substr(1);
    }
    auto hours = parse_duration_fragment<std::chrono::hours, uint64_t, 'H', identifier>(time);
    auto minutes = parse_duration_fragment<std::chrono::minutes, uint64_t, 'M', identifier>(time);
    auto seconds = parse_duration_milliseconds<identifier>(time);

    std::chrono::milliseconds ms{};
    if (days.has_value())
        ms += *days;
    if (!date.empty() || !time.empty())
        throw std::runtime_error{"expected end of string"};
    if (!days.has_value() && !hours.has_value() && !minutes.has_value() && !seconds.has_value()) {
        throw std::runtime_error{"duration without any fields"};
    }
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
bool capabilities::Default<xsd_dayTimeDuration>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    if (value.count() == 0) {
        return writer::write_str("PT0.000S", buffer, cursor, flush);
    }
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
        if (ms_rem.count() != 0)
            str << std::format("{:%S}S", ms_rem);
    }

    return writer::write_str(str.view(), buffer, cursor, flush);
}

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_dayTimeDuration>::try_into_inlined(cpp_type const &value) noexcept {
    int64_t v = value.count();
    return util::try_pack_integral<storage::node::identifier::LiteralID>(v);
}

template<>
capabilities::Inlineable<xsd_dayTimeDuration>::cpp_type capabilities::Inlineable<xsd_dayTimeDuration>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    return std::chrono::milliseconds{util::unpack_integral<int64_t>(inlined)};
}

template<>
std::partial_ordering capabilities::Comparable<xsd_dayTimeDuration>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs <=> rhs;
}

template<>
template<>
capabilities::Subtype<xsd_dayTimeDuration>::super_cpp_type<0> capabilities::Subtype<xsd_dayTimeDuration>::into_supertype<0>(cpp_type const &value) noexcept {
    return std::make_pair(std::chrono::months{0}, value);
}

template<>
template<>
nonstd::expected<capabilities::Subtype<xsd_dayTimeDuration>::cpp_type, DynamicError> capabilities::Subtype<xsd_dayTimeDuration>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    if (value.first.count() == 0 && value.second.count() == 0)
        return value.second;
    if (value.second.count() != 0)
        return value.second;
    return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
}
#endif

template struct LiteralDatatypeImpl<xsd_dayTimeDuration,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable,
                                    capabilities::Subtype>;
}  // namespace rdf4cpp::rdf::datatypes::registry