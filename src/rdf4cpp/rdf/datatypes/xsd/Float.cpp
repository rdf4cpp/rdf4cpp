#include "Float.hpp"
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_float>::cpp_type capabilities::Default<xsd_float>::from_string(std::string_view s) {
    return util::from_chars<cpp_type, identifier>(s);
}

template<>
bool capabilities::Default<xsd_float>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts const writer) noexcept {
    return util::to_chars_canonical(value, writer);
}

template<>
bool capabilities::Default<xsd_float>::serialize_simplified_string(cpp_type const &value, writer::BufWriterParts const writer) noexcept {
    return util::to_chars_simplified(value, writer);
}

template<>
bool capabilities::Logical<xsd_float>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.f;
}

template<>
nonstd::expected<capabilities::Numeric<xsd_float>::abs_result_cpp_type, DynamicError> capabilities::Numeric<xsd_float>::abs(cpp_type const &operand) noexcept {
    return std::abs(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_float>::round_result_cpp_type, DynamicError> capabilities::Numeric<xsd_float>::round(cpp_type const &operand) noexcept {
    return std::round(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_float>::floor_result_cpp_type, DynamicError> capabilities::Numeric<xsd_float>::floor(cpp_type const &operand) noexcept {
    return std::floor(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_float>::ceil_result_cpp_type, DynamicError> capabilities::Numeric<xsd_float>::ceil(cpp_type const &operand) noexcept {
    return std::ceil(operand);
}

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_float>::try_into_inlined(cpp_type const &value) noexcept {
    return util::pack<storage::node::identifier::LiteralID>(value);
}

template<>
capabilities::Inlineable<xsd_float>::cpp_type capabilities::Inlineable<xsd_float>::from_inlined(storage::node::identifier::LiteralID const inlined) noexcept {
    return util::unpack<cpp_type>(inlined);
}
#endif

template struct LiteralDatatypeImpl<xsd_float,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
