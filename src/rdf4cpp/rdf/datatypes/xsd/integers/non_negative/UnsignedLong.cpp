#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedLong.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_unsigned_long>::cpp_type capabilities::Default<xsd_unsigned_long>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_unsigned_long>::to_string(cpp_type const &value) {
    return util::to_chars(value);
}

template<>
bool capabilities::Logical<xsd_unsigned_long>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template struct LiteralDatatypeImpl<xsd_unsigned_long,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
