#include <rdf4cpp/rdf/datatypes/xsd/integers/non_positive/NegativeInteger.hpp>

#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_negative_integer>::cpp_type capabilities::Default<xsd_negative_integer>::from_string(std::string_view s) {
    cpp_type ret;

    try {
        ret = cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"xsd:negativeInteger parsing error: "} + e.what()};
    }

    if (ret > -1) {
        throw std::runtime_error{"xsd:negativeInteger parsing error: found non-negative value"};
    }

    return ret;
}

template<>
bool capabilities::Logical<xsd_negative_integer>::effective_boolean_value(cpp_type const &) noexcept {
    return true;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_negative_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template<>
template<>
nonstd::expected<capabilities::Default<xsd_negative_integer>::cpp_type, DynamicError> capabilities::Subtype<xsd_negative_integer>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    if (value > -1) {
        return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
    }

    return value;
}

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_negative_integer>::try_into_inlined(cpp_type const &value) noexcept {
    auto const to_pack_value = -value - 1;
    if (to_pack_value >= (uint64_t{1} << storage::node::identifier::LiteralID::width)) {
        return std::nullopt;
    }

    auto p = util::try_pack_integral<uint64_t, storage::node::identifier::LiteralID::width>(static_cast<uint64_t>(to_pack_value));
    if (p.has_value())
        return storage::node::identifier::LiteralID{p.value()};
    return std::nullopt;
}

template<>
capabilities::Inlineable<xsd_negative_integer>::cpp_type capabilities::Inlineable<xsd_negative_integer>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    return -cpp_type{util::unpack_integral<uint64_t, storage::node::identifier::LiteralID::width>(inlined.value) + 1};
}

template struct LiteralDatatypeImpl<xsd_negative_integer,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
