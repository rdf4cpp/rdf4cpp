#ifndef RDF4CPP_XSD_UNSIGNED_BYTE_HPP
#define RDF4CPP_XSD_UNSIGNED_BYTE_HPP

#include <rdf4cpp/rdf/datatypes/registry/DatatypeMapping.hpp>
#include <rdf4cpp/rdf/datatypes/registry/LiteralDatatypeImpl.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedShort.hpp>
#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Integer.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>
#include <rdf4cpp/rdf/datatypes/registry/FixedIdMappings.hpp>

#include <cstdint>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
struct DatatypeMapping<xsd_unsigned_byte> {
    using cpp_datatype = uint8_t;
};

template<>
struct DatatypeSupertypeMapping<xsd_unsigned_byte> {
    using supertype = xsd::UnsignedShort;
};

template<>
struct DatatypeNumericStubMapping<xsd_unsigned_byte> {
    using numeric_impl_type = xsd::Integer;
};


template<>
inline capabilities::Default<xsd_unsigned_byte>::cpp_type capabilities::Default<xsd_unsigned_byte>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
inline std::string capabilities::Default<xsd_unsigned_byte>::to_string(cpp_type const &value) {
    return util::to_chars(value);
}


template<>
inline bool capabilities::Logical<xsd_unsigned_byte>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

}  // namespace rdf4cpp::rdf::datatypes::registry


namespace rdf4cpp::rdf::datatypes::xsd {

struct UnsignedByte : registry::LiteralDatatypeImpl<registry::xsd_unsigned_byte,
                                                    registry::capabilities::Logical,
                                                    registry::capabilities::NumericStub,
                                                    registry::capabilities::Subtype,
                                                    registry::capabilities::Comparable,
                                                    registry::capabilities::FixedId> {
};

}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_XSD_UNSIGNED_BYTE_HPP
