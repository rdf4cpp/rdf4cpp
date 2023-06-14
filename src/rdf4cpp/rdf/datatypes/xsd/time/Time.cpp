#include <rdf4cpp/rdf/datatypes/xsd/time/Time.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_time>::cpp_type capabilities::Default<xsd_time>::from_string(std::string_view s) {
    auto p = s.find(':');
    if (p == std::string::npos)
        throw std::invalid_argument("missing :");
    auto hours_s = s.substr(0, p);
    auto min_sec_tz = s.substr(p + 1);
    p = min_sec_tz.find(':');
    if (p == std::string::npos)
        throw std::invalid_argument("missing :");
    auto min_s = min_sec_tz.substr(0, p);
    auto sec_tz = min_sec_tz.substr(p + 1);
    p = sec_tz.find('.');
    std::chrono::milliseconds ms{};
    std::string_view sec_s{};
    OptionalTimezone timezone;
    if (p != std::string::npos) {
        sec_s = sec_tz.substr(0, p);
        auto milli_tz = sec_tz.substr(p + 1);
        auto tz = Timezone::try_parse(milli_tz);
        ms = std::chrono::milliseconds{util::from_chars<unsigned int>(tz.second.substr(0, 3))};
        for (size_t i = tz.second.length(); i < 3; ++i) {
            ms *= 10;
        }
        timezone = tz.first;
    } else {
        auto tz = Timezone::try_parse(sec_tz);
        sec_s = tz.second;
        timezone = tz.first;
    }
    std::chrono::hours h{util::from_chars<unsigned int>(hours_s)};
    std::chrono::minutes m{util::from_chars<unsigned int>(min_s)};
    std::chrono::seconds sec{util::from_chars<unsigned int>(sec_s)};
    auto r = h + m + sec + ms;
    return std::make_pair(r, timezone);
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
    return lhs.first <=> rhs.first;
}

template struct LiteralDatatypeImpl<xsd_time,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry