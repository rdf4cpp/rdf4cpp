#ifndef RDF4CPP_OWL_REAL_HPP
#define RDF4CPP_OWL_REAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>

#include <boost/multiprecision/cpp_bin_float.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

inline constexpr util::ConstexprString owl_real{"http://www.w3.org/2002/07/owl#real"};

template<>
struct DatatypeMapping<owl_real> {
    using cpp_datatype = boost::multiprecision::cpp_bin_float_quad; // IEEE quad float
};

template<>
capabilities::Default<owl_real>::cpp_type capabilities::Default<owl_real>::from_string(std::string_view s);

template<>
bool capabilities::Logical<owl_real>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
std::partial_ordering capabilities::Comparable<owl_real>::compare(cpp_type const &lhs, cpp_type const &rhs);

extern template struct LiteralDatatypeImpl<owl_real,
                                           capabilities::Logical,
                                           capabilities::Numeric,
                                           capabilities::Comparable>;

} // rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::owl {

struct Real : registry::LiteralDatatypeImpl<registry::owl_real,
                                            registry::capabilities::Logical,
                                            registry::capabilities::Numeric,
                                            registry::capabilities::Comparable> {};

} // rdf4cpp::rdf::datatypes::owl

#endif  //RDF4CPP_OWL_REAL_HPP
