#ifndef RDF4CPP_XSD_NEGATIVEINTEGER_HPP
#define RDF4CPP_XSD_NEGATIVEINTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_positive/NonPositiveInteger.hpp>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_negative_integer> {
    using cpp_datatype = boost::multiprecision::cpp_int;
};

template<>
struct DatatypeSupertypeMapping<xsd_negative_integer> {
    using supertype = xsd::NonPositiveInteger;
};

template<>
struct DatatypeNumericStubMapping<xsd_negative_integer> {
    using numeric_impl_type = xsd::Integer;
};

/**
 * Specialisation of from_string template function.
 */
template<>
capabilities::Default<xsd_negative_integer>::cpp_type capabilities::Default<xsd_negative_integer>::from_string(std::string_view s);

template<>
bool capabilities::Logical<xsd_negative_integer>::effective_boolean_value(cpp_type const &) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_negative_integer>::compare(cpp_type const &lhs, cpp_type const &rhs);

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct NegativeInteger : registry::LiteralDatatypeImpl<registry::xsd_negative_integer,
                                                       registry::capabilities::Logical,
                                                       registry::capabilities::NumericStub,
                                                       registry::capabilities::Subtype,
                                                       registry::capabilities::Comparable,
                                                       registry::capabilities::FixedId> {};

} //  rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NEGATIVEINTEGER_HPP
