#include <rdf4cpp/rdf/datatypes/xsd/Double.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_double>::cpp_type capabilities::Default<xsd_double>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_double>::to_string(cpp_type const &value) noexcept {
    return util::to_chars_canonical(value);
}

template<>
std::string capabilities::Default<xsd_double>::display(cpp_type const &value) noexcept {
    return util::to_chars_simplified(value);
}

template<>
bool capabilities::Logical<xsd_double>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.0;
}

template<>
std::optional<uint64_t> capabilities::Inlineable<xsd_double>::try_into_inlined(cpp_type const &value) noexcept {
    static constexpr uint64_t drop_width = 64 - storage::node::identifier::LiteralID::width;

    auto const packed = util::pack<uint64_t>(value);
    auto const shortened = packed >> drop_width; // drop right bits of mantissa (see https://en.wikipedia.org/wiki/Double-precision_floating-point_format)

    if (shortened << drop_width != packed) {
        // dropped part contained information
        return std::nullopt;
    }

    return shortened;
}

template<>
capabilities::Inlineable<xsd_double>::cpp_type capabilities::Inlineable<xsd_double>::from_inlined(uint64_t inlined) noexcept {
    return util::unpack<double>(inlined << (64 - storage::node::identifier::LiteralID::width));
}

template struct LiteralDatatypeImpl<xsd_double,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
