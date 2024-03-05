#include "Double.hpp"
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_double>::cpp_type capabilities::Default<xsd_double>::from_string(std::string_view s) {
    return util::from_chars<cpp_type, identifier>(s);
}

template<>
bool capabilities::Default<xsd_double>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    return util::to_chars_canonical(value, writer);
}

template<>
bool capabilities::Default<xsd_double>::serialize_simplified_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    return util::to_chars_simplified(value, writer);
}

template<>
bool capabilities::Logical<xsd_double>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.0;
}

template<>
nonstd::expected<capabilities::Numeric<xsd_double>::abs_result_cpp_type, DynamicError> capabilities::Numeric<xsd_double>::abs(cpp_type const &operand) noexcept {
    return std::abs(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_double>::round_result_cpp_type, DynamicError> capabilities::Numeric<xsd_double>::round(cpp_type const &operand) noexcept {
    return std::round(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_double>::floor_result_cpp_type, DynamicError> capabilities::Numeric<xsd_double>::floor(cpp_type const &operand) noexcept {
    return std::floor(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_double>::ceil_result_cpp_type, DynamicError> capabilities::Numeric<xsd_double>::ceil(cpp_type const &operand) noexcept {
    return std::ceil(operand);
}

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_double>::try_into_inlined(cpp_type const &value) noexcept {
    static constexpr uint64_t drop_width = 64 - storage::identifier::LiteralID::width;

    auto const packed = util::pack<uint64_t>(value);
    auto const shortened = packed >> drop_width;  // drop right bits of mantissa (see https://en.wikipedia.org/wiki/Double-precision_floating-point_format)

    if (shortened << drop_width != packed) {
        // dropped part contained information
        return std::nullopt;
    }

    return storage::identifier::LiteralID{shortened};
}

template<>
capabilities::Inlineable<xsd_double>::cpp_type capabilities::Inlineable<xsd_double>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    return util::unpack<cpp_type>(inlined.to_underlying() << (64 - storage::identifier::LiteralID::width));
}
#endif

template struct LiteralDatatypeImpl<xsd_double,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
