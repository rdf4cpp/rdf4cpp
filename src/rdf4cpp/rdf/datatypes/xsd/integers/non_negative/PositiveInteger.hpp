#ifndef RDF4CPP_XSD_POSITIVEINTEGER_HPP
#define RDF4CPP_XSD_POSITIVEINTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/NonNegativeInteger.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_positive_integer> {
    using cpp_datatype = boost::multiprecision::cpp_int;
};

template<>
struct DatatypeSupertypeMapping<xsd_positive_integer> {
    using supertype = xsd::NonNegativeInteger;
};

template<>
struct DatatypeNumericStubMapping<xsd_positive_integer> {
    using numeric_impl_type = xsd::Integer;
};

template<>
capabilities::Default<xsd_positive_integer>::cpp_type capabilities::Default<xsd_positive_integer>::from_string(std::string_view s);

template<>
bool capabilities::Logical<xsd_positive_integer>::effective_boolean_value(cpp_type const &) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_positive_integer>::compare(cpp_type const &lhs, cpp_type const &rhs);

extern template struct LiteralDatatypeImpl<xsd_positive_integer,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Comparable,
                                           capabilities::Subtype,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct PositiveInteger : registry::LiteralDatatypeImpl<registry::xsd_positive_integer,
                                                       registry::capabilities::Logical,
                                                       registry::capabilities::NumericStub,
                                                       registry::capabilities::Comparable,
                                                       registry::capabilities::Subtype,
                                                       registry::capabilities::FixedId> {};

} // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_POSITIVEINTEGER_HPP
