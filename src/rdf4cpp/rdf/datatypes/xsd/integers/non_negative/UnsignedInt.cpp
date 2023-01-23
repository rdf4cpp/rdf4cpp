#include <rdf4cpp/rdf/datatypes/xsd/integers/non_negative/UnsignedInt.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_unsigned_int>::cpp_type capabilities::Default<xsd_unsigned_int>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_unsigned_int>::to_string(cpp_type const &value) noexcept {
    return util::to_chars(value);
}

template<>
bool capabilities::Logical<xsd_unsigned_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template struct LiteralDatatypeImpl<xsd_unsigned_int,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Comparable,
                                    capabilities::Subtype,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
