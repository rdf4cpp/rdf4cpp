#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedShort.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_unsigned_short>::cpp_type capabilities::Default<xsd_unsigned_short>::from_string(std::string_view s) {
    return util::from_chars<cpp_type, identifier>(s);
}

template<>
std::string capabilities::Default<xsd_unsigned_short>::to_canonical_string(cpp_type const &value) noexcept {
    return util::to_chars_canonical(value);
}


template<>
bool capabilities::Logical<xsd_unsigned_short>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template struct LiteralDatatypeImpl<xsd_unsigned_short,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
