/**
 * @file Registers xsd:float with DatatypeRegistry
 */

#ifndef RDF4CPP_XSD_FLOAT_HPP
#define RDF4CPP_XSD_FLOAT_HPP


#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/Double.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_float> {
    using cpp_datatype = float;
};

template<>
struct DatatypePromotionMapping<xsd_float> {
    using promoted = xsd::Double;
};

template<>
inline capabilities::Default<xsd_float>::cpp_type capabilities::Default<xsd_float>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
inline std::string capabilities::Default<xsd_float>::to_string(cpp_type const &value) {
    return util::to_chars(value);
}


template<>
inline bool capabilities::Logical<xsd_float>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.f;
}
}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {
/**
 * Implementation of xsd::float
 */
struct Float : registry::LiteralDatatypeImpl<registry::xsd_float,
                                             registry::capabilities::Logical,
                                             registry::capabilities::Numeric,
                                             registry::capabilities::Comparable,
                                             registry::capabilities::Promotable,
                                             registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_FLOAT_HPP
