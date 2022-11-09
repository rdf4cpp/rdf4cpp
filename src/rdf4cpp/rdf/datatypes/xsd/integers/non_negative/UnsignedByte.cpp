#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedByte.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_unsigned_byte>::cpp_type capabilities::Default<xsd_unsigned_byte>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_unsigned_byte>::to_string(cpp_type const &value) noexcept {
    return util::to_chars(value);
}


template<>
bool capabilities::Logical<xsd_unsigned_byte>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template struct LiteralDatatypeImpl<xsd_unsigned_byte,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
