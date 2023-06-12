#include <rdf4cpp/rdf/datatypes/xsd/time/Day.hpp>

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gDay>::cpp_type capabilities::Default<xsd_gDay>::from_string(std::string_view s) {
    auto tz = Timezone::try_parse(s);
    return std::make_pair(std::chrono::day{util::from_chars<unsigned int>(tz.second)}, tz.first);
}

template<>
std::string capabilities::Default<xsd_gDay>::to_canonical_string(const cpp_type &value) noexcept {
    auto str = std::format("{:%d}", value.first);
    if (value.second.has_value())
        str += value.second->to_canonical_string();
    return str;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept{
    return lhs.first <=> rhs.first;
}

template struct LiteralDatatypeImpl<xsd_gDay,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}