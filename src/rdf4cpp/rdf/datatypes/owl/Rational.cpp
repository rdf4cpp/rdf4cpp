#include <rdf4cpp/rdf/datatypes/owl/Rational.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

static owl::Rational::cpp_type simplify(owl::Rational::cpp_type const &r) noexcept {
    auto const divisor = gcd(numerator(r), denominator(r));
    if (divisor == 1) {
        return r;
    }

    return r / divisor;
}

template<>
capabilities::Default<owl_rational>::cpp_type capabilities::Default<owl_rational>::from_string(std::string_view s) {
    if (auto pos = s.find_last_of('-'); pos != std::string_view::npos && pos != 0) {
        // owl:rational only allows - at beginning, boost also allows it in the denominator
        throw std::runtime_error{"owl:rational parsing error: invalid sign position"};
    }

    if (s.find_first_not_of("0123456789/-") != std::string_view::npos) {
        // owl:rational does not allow hex, boost does
        throw std::runtime_error{"owl:rational parsing error: invalid character in string"};
    }

    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"owl:rational parsing error:"} + e.what()};
    }
}

template<>
std::string capabilities::Default<owl_rational>::to_canonical_string(cpp_type const &value) noexcept {
    auto const simplified = simplify(value);

    if (auto den = denominator(simplified); den < 0) {
        // canonicalize x/-y to -x/y and -x/-y to x/y and simplify
        cpp_type const canonical{-numerator(simplified), -std::move(den)};
        return canonical.str();
    }

    return simplified.str();
}

template<>
bool capabilities::Logical<owl_rational>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
std::partial_ordering capabilities::Comparable<owl_rational>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template<>
nonstd::expected<capabilities::Numeric<owl_rational>::abs_result_cpp_type, DynamicError> capabilities::Numeric<owl_rational>::abs(cpp_type const &operand) noexcept {
    return boost::multiprecision::abs(operand);
}

template<>
nonstd::expected<capabilities::Numeric<owl_rational>::round_result_cpp_type, DynamicError> capabilities::Numeric<owl_rational>::round(cpp_type const &operand) noexcept {
    auto const integral_part = numerator(operand) / denominator(operand);
    cpp_type const fractional_part = operand - integral_part;

    if (numerator(fractional_part) * 2 < denominator(fractional_part)) {
        return integral_part;
    }

    return integral_part + 1;
}

template<>
nonstd::expected<capabilities::Numeric<owl_rational>::floor_result_cpp_type, DynamicError> capabilities::Numeric<owl_rational>::floor(cpp_type const &operand) noexcept {
    return static_cast<boost::multiprecision::cpp_int>(operand);
}

template<>
nonstd::expected<capabilities::Numeric<owl_rational>::ceil_result_cpp_type, DynamicError> capabilities::Numeric<owl_rational>::ceil(cpp_type const &operand) noexcept {
    auto const integral = static_cast<boost::multiprecision::cpp_int>(operand);
    auto const rest = numerator(operand) % denominator(operand);
    return integral + (rest != 0);
}

template struct LiteralDatatypeImpl<owl_rational,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable>;

} // rdf4cpp::rdf::datatypes::registry
