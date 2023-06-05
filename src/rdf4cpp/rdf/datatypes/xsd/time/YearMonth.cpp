#include <fmt/format.h>
#include <fmt/chrono.h>

#include <rdf4cpp/rdf/datatypes/xsd/time/YearMonth.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gYearMonth>::cpp_type capabilities::Default<xsd_gYearMonth>::from_string(std::string_view s) {
    auto p = s.find('-');
    if (p == std::string::npos)
        throw std::invalid_argument("missing -");
    std::chrono::year y{util::from_chars<int>(s.substr(0, p))};
    std::chrono::month m{util::from_chars<unsigned int>(s.substr(p+1))};
    auto r = y/m;
    if (!r.ok())
        throw std::invalid_argument("invalid month_year");
    return r;
}

template<>
std::string capabilities::Default<xsd_gYearMonth>::to_canonical_string(const cpp_type &value) noexcept {
    return fmt::format("{}-{}", static_cast<int>(value.year()), static_cast<unsigned int>(value.month()));
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gYearMonth>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept{
    return lhs <=> rhs;
}

template struct LiteralDatatypeImpl<xsd_gYearMonth,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}