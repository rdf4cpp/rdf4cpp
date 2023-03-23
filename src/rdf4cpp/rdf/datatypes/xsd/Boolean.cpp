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
    return storage::node::identifier::LiteralID{util::pack<uint64_t>(value)};
}

template<>
capabilities::Inlineable<xsd_boolean>::cpp_type capabilities::Inlineable<xsd_boolean>::from_inlined(storage::node::identifier::LiteralID inlined) noexcept {
    return util::unpack<bool>(inlined.value);
}

template struct LiteralDatatypeImpl<xsd_boolean,
                                    capabilities::Logical,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
