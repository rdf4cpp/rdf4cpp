#include "NonPositiveInteger.hpp"

#include <stdexcept>

#include <rdf4cpp/InvalidNode.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_non_positive_integer>::cpp_type capabilities::Default<xsd_non_positive_integer>::from_string(std::string_view s) {
    cpp_type ret;

    try {
        ret = cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw InvalidNode{std::string{"xsd:nonPositiveInteger parsing error: "} + e.what()};
    }

    if (ret > 0) {
        throw InvalidNode{"xsd:nonPositiveInteger parsing error: found non-negative value"};
    }

    return ret;
}

template<>
bool capabilities::Logical<xsd_non_positive_integer>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
std::partial_ordering capabilities::Comparable<xsd_non_positive_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
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
nonstd::expected<capabilities::Default<xsd_non_positive_integer>::cpp_type, DynamicError> capabilities::Subtype<xsd_non_positive_integer>::from_supertype<0>(super_cpp_type<0> const &value) noexcept {
    if (value > 0) {
        return nonstd::make_unexpected(DynamicError::InvalidValueForCast);
    }

    return value;
}

template<>
std::optional<storage::identifier::LiteralID> capabilities::Inlineable<xsd_non_positive_integer>::try_into_inlined(cpp_type const &value) noexcept {
    auto const to_pack_value = -value;
    if (to_pack_value >= (uint64_t{1} << storage::identifier::LiteralID::width)) {
        return std::nullopt;
    }

    return util::try_pack_integral<storage::identifier::LiteralID>(static_cast<uint64_t>(to_pack_value));
}

template<>
capabilities::Inlineable<xsd_non_positive_integer>::cpp_type capabilities::Inlineable<xsd_non_positive_integer>::from_inlined(storage::identifier::LiteralID inlined) noexcept {
    return -cpp_type{util::unpack_integral<uint64_t>(inlined)};
}
#endif

template struct LiteralDatatypeImpl<xsd_non_positive_integer,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::datatypes::registry
