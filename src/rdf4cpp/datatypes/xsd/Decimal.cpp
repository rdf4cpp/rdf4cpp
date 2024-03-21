#include "Decimal.hpp"

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <regex>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
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
bool capabilities::Default<xsd_decimal>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    auto const s = static_cast<std::string>(value);
    return writer::write_str(s, writer);
}

template<>
bool capabilities::Default<xsd_decimal>::serialize_simplified_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    cpp_type v = value;
    v.normalize();
    if (v.get_exponent() == 0) {
        auto const s = static_cast<boost::multiprecision::cpp_int>(v).str();
        return writer::write_str(s, writer);
    } else {
        auto const s = static_cast<std::string>(v);
        return writer::write_str(s, writer);
    }
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::add_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::add(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#op.numeric
    // decimal needs overflow protection
    auto r = lhs.add_checked(rhs);
    if (r.has_value())
        return r.value();
    else
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::sub_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::sub(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#op.numeric
    // decimal needs overflow protection

    auto r = lhs.sub_checked(rhs);
    if (r.has_value())
        return r.value();
    else
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::div_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#func-numeric-divide
    // decimal needs error (and cpp_type is not integral) && overflow protection

    if (rhs == 0) {
        return nonstd::make_unexpected(DynamicError::DivideByZero);
    }

    auto r = lhs.div_checked(rhs, 1000);
    if (r.has_value())
        return r.value();
    else
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::mul_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::mul(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#op.numeric
    // decimal needs overflow protection

    auto r = lhs.mul_checked(rhs);
    if (r.has_value())
        return r.value();
    else
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::abs_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::neg(cpp_type const &operand) noexcept {
    auto r = operand.unary_minus_checked();
    if (r.has_value())
        return r.value();
    else
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::abs_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::abs(cpp_type const &operand) noexcept {
    auto r = operand.abs_checked();
    if (r.has_value())
        return r.value();
    else
        return nonstd::make_unexpected(DynamicError::OverOrUnderFlow);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::round_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::round(cpp_type const &operand) noexcept {
    return operand.round(rdf4cpp::RoundingMode::Round);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::floor_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::floor(cpp_type const &operand) noexcept {
    return operand.round(rdf4cpp::RoundingMode::Floor);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::ceil_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::ceil(cpp_type const &operand) noexcept {
    return operand.round(rdf4cpp::RoundingMode::Ceil);
}

template<>
bool capabilities::Logical<xsd_decimal>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_decimal>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    return lhs <=> rhs;
}
#endif

template struct LiteralDatatypeImpl<xsd_decimal,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::datatypes::registry
