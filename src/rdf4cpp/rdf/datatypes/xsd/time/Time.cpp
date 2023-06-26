#include <rdf4cpp/rdf/datatypes/xsd/time/Time.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_time>::cpp_type capabilities::Default<xsd_time>::from_string(std::string_view s) {
    auto hours = parse_date_time_fragment<std::chrono::hours, unsigned int, ':'>(s);
    auto minutes = parse_date_time_fragment<std::chrono::minutes, unsigned int, ':'>(s);
    auto tz = Timezone::try_parse(s);
    std::chrono::milliseconds ms = parse_milliseconds(tz.second);
    auto time = hours + minutes + ms;
    if (time > std::chrono::hours{24})
        throw std::invalid_argument{"invalid time of day"};

    return std::make_pair(time, tz.first);
}

template<>
std::string capabilities::Default<xsd_time>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%H:%M:%S}", std::chrono::hh_mm_ss(value.first));
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_time>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return TimeComparer<std::chrono::milliseconds>::compare(lhs.first, lhs.second, rhs.first, rhs.second);
}

template<>
TimePoint to_timePoint<std::chrono::milliseconds>(std::chrono::milliseconds t) {
    return construct(TimePointReplacementDate, t);
}

template struct LiteralDatatypeImpl<xsd_time,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry