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
std::partial_ordering capabilities::Comparable<xsd_negative_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template struct LiteralDatatypeImpl<xsd_negative_integer,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

template ConversionTable auto make_conversion_table_for<xsd::NegativeInteger>();

}  // namespace rdf4cpp::rdf::datatypes::registry
