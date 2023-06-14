#include <rdf4cpp/rdf/datatypes/xsd/time/Date.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_date>::cpp_type capabilities::Default<xsd_date>::from_string(std::string_view s) {
    auto p = s.find('-');
    if (p == std::string::npos)
        throw std::invalid_argument("missing -");
    auto year_s = s.substr(0, p);
    auto month_day_tz = s.substr(p + 1);
    p = month_day_tz.find('-');
    if (p == std::string::npos)
        throw std::invalid_argument("missing -");
    auto month_s = month_day_tz.substr(0, p);
    auto day_tz = month_day_tz.substr(p + 1);
    auto tz = Timezone::try_parse(day_tz);
    std::chrono::year y{util::from_chars<int>(year_s)};
    std::chrono::month m{util::from_chars<unsigned int>(month_s)};
    std::chrono::day d{util::from_chars<unsigned int>(tz.second)};
    auto r = y / m / d;
    if (!r.ok())
        throw std::invalid_argument("invalid date");
    return std::make_pair(r, tz.first);
}

template<>
std::string capabilities::Default<xsd_date>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%Y-%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_date>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs.first <=> rhs.first;
}

template struct LiteralDatatypeImpl<xsd_date,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry