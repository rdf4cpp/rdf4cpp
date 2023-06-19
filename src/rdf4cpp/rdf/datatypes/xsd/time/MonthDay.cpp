#include <rdf4cpp/rdf/datatypes/xsd/time/MonthDay.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gMonthDay>::cpp_type capabilities::Default<xsd_gMonthDay>::from_string(std::string_view s) {
    auto month = parse_date_time_fragment<std::chrono::month, unsigned int, '-'>(s);
    auto tz = Timezone::try_parse(s);
    auto day = parse_date_time_fragment<std::chrono::day, unsigned int, '\0'>(tz.second);
    auto date = month / day;
    if (!date.ok())
        throw std::invalid_argument("invalid date");

    return std::make_pair(date, tz.first);
}

template<>
std::string capabilities::Default<xsd_gMonthDay>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%m-%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gMonthDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs.first <=> rhs.first;
}

template struct LiteralDatatypeImpl<xsd_gMonthDay,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}  // namespace rdf4cpp::rdf::datatypes::registry