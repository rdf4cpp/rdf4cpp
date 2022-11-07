#include <rdf4cpp/rdf/datatypes/xsd/Boolean.hpp>

#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_boolean>::cpp_type capabilities::Default<xsd_boolean>::from_string(std::string_view s) {
    if (s == "true" || s == "1") {
        return true;
    } else if (s == "false" || s == "0") {
        return false;
    } else {
        throw std::runtime_error{"XSD Parsing Error"};
    }
}

template<>
std::string capabilities::Default<xsd_boolean>::to_string(const cpp_type &value) {
    if (value)  {
        return "true";
    } else {
        return "false";
    }
}

template<>
bool capabilities::Logical<xsd_boolean>::effective_boolean_value(cpp_type const &value) noexcept {
    return value;
}

template struct LiteralDatatypeImpl<xsd_boolean,
                                    capabilities::Logical,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
