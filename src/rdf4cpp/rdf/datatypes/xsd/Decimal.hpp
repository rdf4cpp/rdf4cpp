#ifndef RDF4CPP_XSD_DECIMAL_HPP
#define RDF4CPP_XSD_DECIMAL_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Float.hpp>

#include <boost/multiprecision/cpp_dec_float.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_decimal> {
    // needs at least 18 decimal digits of precision
    // see: https://www.w3.org/TR/2004/REC-xmlschema-2-20041028/#dt-decimal
    using cpp_datatype = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<18, int16_t>>;
};

template<>
struct DatatypePromotionMapping<xsd_decimal> {
    using promoted = xsd::Float;
};

template<>
capabilities::Default<xsd_decimal>::cpp_type capabilities::Default<xsd_decimal>::from_string(std::string_view s);

template<>
std::string capabilities::Default<xsd_decimal>::to_string(const cpp_type &value);

template<>
nonstd::expected<capabilities::Numeric<xsd_decimal>::div_result_cpp_type, NumericOpError> capabilities::Numeric<xsd_decimal>::div(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
bool capabilities::Logical<xsd_decimal>::effective_boolean_value(cpp_type const &value) noexcept;

template<>
std::partial_ordering capabilities::Comparable<xsd_decimal>::compare(cpp_type const &lhs, cpp_type const &rhs);

extern template struct LiteralDatatypeImpl<xsd_decimal,
                                           capabilities::Logical,
                                           capabilities::Numeric,
                                           capabilities::Comparable,
                                           capabilities::Promotable,
                                           capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry

namespace rdf4cpp::rdf::datatypes::xsd {

struct Decimal : registry::LiteralDatatypeImpl<registry::xsd_decimal,
                                               registry::capabilities::Logical,
                                               registry::capabilities::Numeric,
                                               registry::capabilities::Comparable,
                                               registry::capabilities::Promotable,
                                               registry::capabilities::FixedId> {};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_DECIMAL_HPP
