/**
 * @file Registers xsd:string with DatatypeRegistry
 */

#include <rdf4cpp/rdf/datatypes/xsd/String.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_string>::cpp_type capabilities::Default<xsd_string>::from_string(std::string_view s) {
    return std::string{s};
}

template<>
std::string capabilities::Default<xsd_string>::to_string(cpp_type const &value) {
    return value;
}

template<>
bool capabilities::Logical<xsd_string>::effective_boolean_value(cpp_type const &value) noexcept {
    return !value.empty();
}

template struct LiteralDatatypeImpl<xsd_string,
                                    capabilities::Logical,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
