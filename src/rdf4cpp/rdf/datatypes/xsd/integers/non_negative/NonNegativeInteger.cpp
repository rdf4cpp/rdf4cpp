#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/NonNegativeInteger.hpp>

#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_non_negative_integer>::cpp_type capabilities::Default<xsd_non_negative_integer>::from_string(std::string_view s) {
    cpp_type ret;

    try {
        ret = cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"xsd:nonNegativeInteger parsing error: "} + e.what()};
    }

    if (ret < 0) {
        throw std::runtime_error{"xsd:nonNegativeInteger parsing error: found negative value"};
    }

    return ret;
}

template<>
bool capabilities::Logical<xsd_non_negative_integer>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_non_negative_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template<>
nonstd::expected<capabilities::Default<xsd_non_negative_integer>::cpp_type, DynamicError> capabilities::Subtype<xsd_non_negative_integer>::from_supertype(super_cpp_type const &value) noexcept {
    if (value < 0) {
        return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
    }

    return value;
}

template struct LiteralDatatypeImpl<xsd_non_negative_integer,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Comparable,
                                    capabilities::Subtype,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry