#include <rdf4cpp/rdf/datatypes/xsd/Float.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

#include <cmath>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_float>::cpp_type capabilities::Default<xsd_float>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_float>::to_string(cpp_type const &value) noexcept {
    return util::to_chars(value);
}


template<>
bool capabilities::Logical<xsd_float>::effective_boolean_value(cpp_type const &value) noexcept {
    return !std::isnan(value) && value != 0.f;
}

template struct LiteralDatatypeImpl<xsd_float,
                                    capabilities::Logical,
                                    capabilities::Numeric,
                                    capabilities::Comparable,
                                    capabilities::Promotable,
                                    capabilities::FixedId>;

template ConversionTable auto make_conversion_table_for<xsd::Float>();

}  // namespace rdf4cpp::rdf::datatypes::registry
