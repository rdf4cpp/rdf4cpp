#ifndef RDF4CPP_OWL_RATIONAL_HPP
#define RDF4CPP_OWL_RATIONAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

inline constexpr util::ConstexprString owl_rational{"http://www.w3.org/2002/07/owl#rational"};

template<>
struct DatatypeMapping<owl_rational> {
    using cpp_datatype = boost::multiprecision::cpp_rational;
};

template<>
inline capabilities::Default<owl_rational>::cpp_type capabilities::Default<owl_rational>::from_string(std::string_view s) {
    if (auto pos = s.find_last_of('-'); pos != std::string_view::npos && pos != 0) {
        // owl:rational only allows - at beginning
        throw std::runtime_error{"owl:rational parsing error: invalid sign position"};
    }

    if (s.find_first_not_of("0123456789/-") != std::string_view::npos) {
        throw std::runtime_error{"owl:rational parsing error: invalid character in string"};
    }

    try {
        return cpp_type{s};
    } catch (std::runtime_error const &e) {
        throw std::runtime_error{std::string{"owl:rational parsing error:"} + e.what()};
    }
}

template<>
inline std::string capabilities::Default<owl_rational>::to_string(cpp_type const &value) {
    std::ostringstream oss;
    oss << numerator(value) << '/' << denominator(value);
    return oss.str();
}

template<>
inline bool capabilities::Logical<owl_rational>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
inline std::partial_ordering capabilities::Comparable<owl_rational>::compare(cpp_type const &lhs, cpp_type const &rhs) {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

} // rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::owl {

struct Rational : registry::LiteralDatatypeImpl<registry::owl_rational,
                                                registry::capabilities::Logical,
                                                registry::capabilities::Numeric,
                                                registry::capabilities::Comparable> {
};

} // rdf4cpp::rdf::datatypes::owl

#endif  //RDF4CPP_OWL_RATIONAL_HPP
