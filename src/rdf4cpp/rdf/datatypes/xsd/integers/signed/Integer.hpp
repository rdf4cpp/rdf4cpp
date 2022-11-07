#ifndef RDF4CPP_XSD_INTEGER_HPP
#define RDF4CPP_XSD_INTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Decimal.hpp>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_integer> {
    using cpp_datatype = boost::multiprecision::cpp_int;
};

template<>
struct DatatypeSupertypeMapping<xsd_integer> {
    using supertype = xsd::Decimal;
};

template<>
struct DatatypeDivResultMapping<xsd_integer> {
    using op_result = xsd::Decimal;
};

template<>
capabilities::Default<xsd_integer>::cpp_type capabilities::Default<xsd_integer>::from_string(std::string_view s);

template<>
bool capabilities::Logical<xsd_integer>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
nonstd::expected<capabilities::Numeric<xsd_integer>::div_result_cpp_type, NumericOpError> capabilities::Numeric<xsd_integer>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_integer>::compare(cpp_type const &lhs, cpp_type const &rhs);

extern template struct LiteralDatatypeImpl<xsd_integer,
                                           capabilities::Logical,
                                           capabilities::Numeric,
                                           capabilities::Comparable,
                                           capabilities::Subtype,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct Integer : registry::LiteralDatatypeImpl<registry::xsd_integer,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Numeric,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::Subtype,
                                               registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd
#endif  //RDF4CPP_XSD_INTEGER_HPP
