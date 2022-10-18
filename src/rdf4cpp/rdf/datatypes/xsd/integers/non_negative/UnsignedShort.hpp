#ifndef RDF4CPP_XSD_UNSIGNED_SHORT_HPP
#define RDF4CPP_XSD_UNSIGNED_SHORT_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedInt.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_unsigned_short> {
    using cpp_datatype = uint16_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_unsigned_short> {
    using supertype = xsd::UnsignedInt;
};

template<>
struct DatatypeNumericStubMapping<xsd_unsigned_short> {
    using numeric_impl_type = xsd::Integer;
};


template<>
inline capabilities::Default<xsd_unsigned_short>::cpp_type capabilities::Default<xsd_unsigned_short>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
inline std::string capabilities::Default<xsd_unsigned_short>::to_string(cpp_type const &value) {
    return util::to_chars(value);
}


template<>
inline bool capabilities::Logical<xsd_unsigned_short>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct UnsignedShort : registry::LiteralDatatypeImpl<registry::xsd_unsigned_short,
                                                     registry::capabilities::Logical,
                                                     registry::capabilities::NumericStub,
                                                     registry::capabilities::Subtype,
                                                     registry::capabilities::Comparable,
                                                     registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_UNSIGNED_SHORT_HPP