#include <rdf4cpp/rdf/datatypes/xsd/time/day.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_gDay>::cpp_type capabilities::Default<xsd_gDay>::from_string(std::string_view s) {
    return std::chrono::day{util::from_chars<unsigned int>(s)};
}

template<>
std::string capabilities::Default<xsd_gDay>::to_canonical_string(const cpp_type &value) noexcept {
    return util::to_chars_canonical(static_cast<unsigned int>(value));
}

template<>
std::partial_ordering capabilities::Comparable<xsd_gDay>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept{
    if ( lhs == rhs)
        return std::partial_ordering::equivalent;
    else if (lhs < rhs)
        return std::partial_ordering::less;
    else
        return std::partial_ordering::greater;
}

template struct LiteralDatatypeImpl<xsd_gDay,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;
}