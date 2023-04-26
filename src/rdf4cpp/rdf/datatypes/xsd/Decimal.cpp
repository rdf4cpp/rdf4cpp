#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_decimal>::cpp_type capabilities::Default<xsd_decimal>::from_string(std::string_view s) {
    // https://www.w3.org/TR/xmlschema11-2/#decimal
    static std::regex const decimal_regex{R"#((\+|-)?([0-9]+(\.[0-9]*)?|\.[0-9]+))#", std::regex_constants::optimize};

    if (!std::regex_match(s.begin(), s.end(), decimal_regex)) {
        throw std::runtime_error{"XSD Parsing Error"};
    }

    if (s.starts_with('+')) {
        s.remove_prefix(1);
    }

    return cpp_type{s};
}

template<>
std::string capabilities::Default<xsd_decimal>::to_canonical_string(cpp_type const &value) noexcept {
    return static_cast<std::string>(value);
}

template<>
std::string capabilities::Default<xsd_decimal>::to_simplified_string(cpp_type const &value) noexcept {
    return static_cast<std::string>(value);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::add_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::add(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#op.numeric
    // decimal needs overflow protection

    try {
        return lhs + rhs;
    } catch (const std::overflow_error &) {
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
    }
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::sub_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::sub(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#op.numeric
    // decimal needs overflow protection

    try {
        return lhs - rhs;
    } catch (const std::overflow_error &) {
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
    }
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::div_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#func-numeric-divide
    // decimal needs error (and cpp_type is not integral) && overflow protection

    if (rhs == 0) {
        return nonstd::make_unexpected(DynamicError::DivideByZero);
    }

    try {
        return lhs / rhs;
    } catch (const std::overflow_error &) {
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
    }
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::mul_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::mul(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#op.numeric
    // decimal needs overflow protection

    try {
        return lhs * rhs;
    } catch (const std::overflow_error &) {
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
    }
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::abs_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::abs(cpp_type const &operand) noexcept {
    return operand.abs();
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::round_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::round(cpp_type const &operand) noexcept {
    return operand.round(rdf4cpp::rdf::util::RoundingMode::Round);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::floor_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::floor(cpp_type const &operand) noexcept {
    return operand.round(rdf4cpp::rdf::util::RoundingMode::Floor);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::ceil_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::ceil(cpp_type const &operand) noexcept {
    return operand.round(rdf4cpp::rdf::util::RoundingMode::Ceil);
}

template<>
bool capabilities::Logical<xsd_decimal>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_decimal>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs <=> rhs;
}

template struct LiteralDatatypeImpl<xsd_decimal,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
