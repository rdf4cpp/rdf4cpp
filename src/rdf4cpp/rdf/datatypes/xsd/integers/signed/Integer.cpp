#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>

#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_integer>::cpp_type capabilities::Default<xsd_integer>::from_string(std::string_view s) {
    if (s.starts_with('+')) {
        s.remove_prefix(1);
    }

    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"xsd:integer parsing error: "} + e.what()};
    }
}

template<>
bool capabilities::Logical<xsd_integer>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
nonstd::expected<capabilities::Numeric<xsd_integer>::div_result_cpp_type, DynamicError> capabilities::Numeric<xsd_integer>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (rhs == 0) {
        return nonstd::make_unexpected(DynamicError::DivideByZero);
    }

    // https://www.w3.org/TR/xpath-functions/#func-numeric-divide
    // integer needs to return decimal on division
    return static_cast<div_result_cpp_type>(lhs) / static_cast<div_result_cpp_type>(rhs);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_integer>::abs_result_cpp_type, DynamicError> capabilities::Numeric<xsd_integer>::abs(cpp_type const &operand) noexcept {
    return boost::multiprecision::abs(operand);
}

template<>
nonstd::expected<capabilities::Numeric<xsd_integer>::round_result_cpp_type, DynamicError> capabilities::Numeric<xsd_integer>::round(cpp_type const &operand) noexcept {
    return operand;
}

template<>
nonstd::expected<capabilities::Numeric<xsd_integer>::floor_result_cpp_type, DynamicError> capabilities::Numeric<xsd_integer>::floor(cpp_type const &operand) noexcept {
    return operand;
}

template<>
nonstd::expected<capabilities::Numeric<xsd_integer>::ceil_result_cpp_type, DynamicError> capabilities::Numeric<xsd_integer>::ceil(cpp_type const &operand) noexcept {
    return operand;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_integer>::try_into_inlined(cpp_type const &value) noexcept {
    if (auto const boundary = 1l << (storage::node::identifier::LiteralID::width - 1); value >= boundary || value < -boundary) [[unlikely]] {
        return std::nullopt;
    }

    auto p = util::try_pack_integral<uint64_t, storage::node::identifier::LiteralID::width>(static_cast<int64_t>(value));
    if (p.has_value())
        return storage::node::identifier::LiteralID{p.value()};
    return std::nullopt;
}

template<>
capabilities::Inlineable<xsd_integer>::cpp_type capabilities::Inlineable<xsd_integer>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    return cpp_type{util::unpack_integral<int64_t, storage::node::identifier::LiteralID::width>(inlined.value)};
}

template struct LiteralDatatypeImpl<xsd_integer,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Subtype,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
