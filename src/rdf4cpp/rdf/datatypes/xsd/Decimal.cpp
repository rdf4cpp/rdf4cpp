#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <regex>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_decimal>::cpp_type capabilities::Default<xsd_decimal>::from_string(std::string_view s) {
    static std::regex const decimal_regex{R"#((\+|-)?[0-9]+\.[0-9]*)#"};

    if (!std::regex_match(s.begin(), s.end(), decimal_regex)) {
        throw std::runtime_error{"XSD Parsing Error"};
    }

    if (s.starts_with('+')) {
        s.remove_prefix(1);
    }

    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"xsd:decimal parsing error: "} + e.what()};
    }
}

template<>
std::string capabilities::Default<xsd_decimal>::to_string(const cpp_type &value) noexcept {
    auto s = value.str(std::numeric_limits<cpp_type>::digits10, std::ios_base::fixed | std::ios_base::showpoint);
    auto const non_zero_pos = s.find_last_not_of('0');

    // cannot be npos because, showpoint is set and '.' != '0'
    assert(non_zero_pos != std::string::npos);

    // dot was found char implies there is a char after the dot (because precision == digits10 > 0) and char after dot must be zero
    assert(s[non_zero_pos] != '.' || (s.size() >= non_zero_pos + 2 && s[non_zero_pos + 1] == '0'));

    // +1 for pos -> size conversion
    // maybe +1 to include one zero after dot
    s.resize(non_zero_pos + 1 + static_cast<std::string::size_type>(s[non_zero_pos] == '.'));

    return s;
}

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::div_result_cpp_type, DynamicError> capabilities::Numeric<xsd_decimal>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    // https://www.w3.org/TR/xpath-functions/#func-numeric-divide
    // decimal needs error (and cpp_type is not integral)

    if (rhs == 0) {
        return nonstd::make_unexpected(DynamicError::DivideByZero);
    }

    return lhs / rhs;
}

template<>
bool capabilities::Logical<xsd_decimal>::effective_boolean_value(cpp_type const &value) noexcept {
    return !isnan(value) && value != 0.0;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_decimal>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template struct LiteralDatatypeImpl<xsd_decimal,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
