#include <rdf4cpp/rdf/datatypes/xsd/Float.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_float>::cpp_type capabilities::Default<xsd_float>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_float>::to_canonical_string(cpp_type const &value) noexcept {
    return util::to_chars_canonical(value);
}

template<>
std::string capabilities::Default<xsd_float>::to_simplified_string(cpp_type const &value) noexcept {
    return util::to_chars_simplified(value);
}

template<>
bool capabilities::Logical<xsd_float>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.f;
}

template<>
std::optional<uint64_t> capabilities::Inlineable<xsd_float>::try_into_inlined(cpp_type const &value) noexcept {
    return util::pack<uint64_t>(value);
}

template<>
capabilities::Inlineable<xsd_float>::cpp_type capabilities::Inlineable<xsd_float>::from_inlined(uint64_t const inlined) noexcept {
    return util::unpack<cpp_type>(inlined);
}

template struct LiteralDatatypeImpl<xsd_float,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
