#include <rdf4cpp/rdf/datatypes/owl/Real.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<owl_real>::cpp_type capabilities::Default<owl_real>::from_string(std::string_view s) {
    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"owl:real parsing error:"} + e.what()};
    }
}

template<>
bool capabilities::Logical<owl_real>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
std::partial_ordering capabilities::Comparable<owl_real>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

template struct LiteralDatatypeImpl<owl_real,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable>;

} // rdf4cpp::rdf::datatypes::registry
