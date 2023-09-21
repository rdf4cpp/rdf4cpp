#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Int.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/Inlining.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_int>::cpp_type capabilities::Default<xsd_int>::from_string(std::string_view s) {
    return util::from_chars<cpp_type, identifier>(s);
}

template<>
std::string capabilities::Default<xsd_int>::to_canonical_string(cpp_type const &value) noexcept {
    return util::to_chars_canonical(value);
}

template<>
bool capabilities::Logical<xsd_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template struct LiteralDatatypeImpl<xsd_int,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Comparable,
                                    capabilities::Subtype,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
