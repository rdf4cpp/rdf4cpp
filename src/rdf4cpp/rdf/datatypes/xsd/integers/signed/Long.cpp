#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Long.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_long>::cpp_type capabilities::Default<xsd_long>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_long>::to_canonical_string(cpp_type const &value) noexcept {
    return util::to_chars_canonical(value);
}

template<>
bool capabilities::Logical<xsd_long>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
nonstd::expected<capabilities::Default<xsd_long>::cpp_type, DynamicError> capabilities::Subtype<xsd_long>::from_supertype(super_cpp_type const &value) noexcept {
    if (value > std::numeric_limits<cpp_type>::max() || value < std::numeric_limits<cpp_type>::min()) {
        return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
    }

    return static_cast<cpp_type>(value);
}

template struct LiteralDatatypeImpl<xsd_long,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
