#include "Boolean.hpp"

#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
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
std::string capabilities::Default<xsd_boolean>::to_canonical_string(cpp_type const &value) noexcept {
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

template<>
std::optional<storage::node::identifier::LiteralID> capabilities::Inlineable<xsd_boolean>::try_into_inlined(cpp_type const &value) noexcept {
    return util::pack<storage::node::identifier::LiteralID>(value);
}

template<>
capabilities::Inlineable<xsd_boolean>::cpp_type capabilities::Inlineable<xsd_boolean>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    return util::unpack<cpp_type>(inlined);
}
#endif

template struct LiteralDatatypeImpl<xsd_boolean,
                                    capabilities::Logical,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
