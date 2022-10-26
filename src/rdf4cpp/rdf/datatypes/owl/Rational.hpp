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
capabilities::Default<owl_rational>::cpp_type capabilities::Default<owl_rational>::from_string(std::string_view s);

template<>
std::string capabilities::Default<owl_rational>::to_string(cpp_type const &value);

template<>
bool capabilities::Logical<owl_rational>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
std::partial_ordering capabilities::Comparable<owl_rational>::compare(cpp_type const &lhs, cpp_type const &rhs);

extern template struct LiteralDatatypeImpl<owl_rational,
                                           capabilities::Logical,
                                           capabilities::Numeric,
                                           capabilities::Comparable>;

} // rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::owl {

struct Rational : registry::LiteralDatatypeImpl<registry::owl_rational,
                                                registry::capabilities::Logical,
                                                registry::capabilities::Numeric,
                                                registry::capabilities::Comparable> {};

} // rdf4cpp::rdf::datatypes::owl

#endif  //RDF4CPP_OWL_RATIONAL_HPP
