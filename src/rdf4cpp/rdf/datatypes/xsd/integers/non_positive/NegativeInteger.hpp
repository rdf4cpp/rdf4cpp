#ifndef RDF4CPP_XSD_NEGATIVEINTEGER_HPP
#define RDF4CPP_XSD_NEGATIVEINTEGER_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_positive/NonPositiveInteger.hpp>

#include <boost/multiprecision/cpp_int.hpp>

#include <stdexcept>

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
inline capabilities::Default<xsd_negative_integer>::cpp_type capabilities::Default<xsd_negative_integer>::from_string(std::string_view s) {
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
inline bool capabilities::Logical<xsd_negative_integer>::effective_boolean_value(cpp_type const &) noexcept {
    return true;
}

template<>
inline std::partial_ordering capabilities::Comparable<xsd_negative_integer>::compare(cpp_type const &lhs, cpp_type const &rhs) {
    if (lhs < rhs) {
        return std::partial_ordering::less;
    } else if (rhs < lhs) {
        return std::partial_ordering::greater;
    } else {
        return std::partial_ordering::equivalent;
    }
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct NegativeInteger : registry::LiteralDatatypeImpl<registry::xsd_negative_integer,
                                                       registry::capabilities::Logical,
                                                       registry::capabilities::NumericStub,
                                                       registry::capabilities::Subtype,
                                                       registry::capabilities::Comparable,
                                                       registry::capabilities::FixedId> {
};

} //  rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_NEGATIVEINTEGER_HPP
