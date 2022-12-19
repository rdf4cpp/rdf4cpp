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
std::partial_ordering capabilities::Comparable<xsd_negative_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) noexcept;

template<>
nonstd::expected<capabilities::Default<xsd_negative_integer>::cpp_type, DynamicError> capabilities::Subtype<xsd_negative_integer>::from_supertype(super_cpp_type const &value) noexcept;

template<>
std::optional<uint64_t> capabilities::Inlineable<xsd_negative_integer>::try_into_inlined(cpp_type const &value) noexcept;

template<>
capabilities::Inlineable<xsd_negative_integer>::cpp_type capabilities::Inlineable<xsd_negative_integer>::from_inlined(uint64_t inlined) noexcept;

extern template struct LiteralDatatypeImpl<xsd_negative_integer,
                                           capabilities::Logical,
                                           capabilities::NumericStub,
                                           capabilities::Subtype,
                                           capabilities::Comparable,
                                           capabilities::FixedId,
                                           capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct NegativeInteger : registry::LiteralDatatypeImpl<registry::xsd_negative_integer,
                                                       registry::capabilities::Logical,
                                                       registry::capabilities::NumericStub,
                                                       registry::capabilities::Subtype,
                                                       registry::capabilities::Comparable,
                                                       registry::capabilities::FixedId,
                                                       registry::capabilities::Inlineable> {};

} //  rdf4cpp::rdf::datatypes::xsd


namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail {

[[maybe_unused]] inline xsd::NegativeInteger const xsd_negative_integer_instance;

} // namespace rdf4cpp::rdf::datatypes::registry::instantiation_detail

#endif  //RDF4CPP_XSD_NEGATIVEINTEGER_HPP
