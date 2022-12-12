#include <rdf4cpp/rdf/datatypes/xsd/integers/signed/Int.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/Inlining.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template<>
capabilities::Default<xsd_int>::cpp_type capabilities::Default<xsd_int>::from_string(std::string_view s) {
    return util::from_chars<cpp_type>(s);
}

template<>
std::string capabilities::Default<xsd_int>::to_string(cpp_type const &value) noexcept {
    return util::to_chars(value);
}

template<>
bool capabilities::Logical<xsd_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}

template<>
bool capabilities::Inlineable<xsd_int>::can_inline(cpp_type const &) noexcept {
    return true;
}

template<>
uint64_t capabilities::Inlineable<xsd_int>::to_inlined(cpp_type const &value) noexcept {
    return util::pack<uint64_t>(value);
}

template<>
capabilities::Inlineable<xsd_int>::cpp_type capabilities::Inlineable<xsd_int>::from_inlined(uint64_t const inlined) noexcept {
    return util::unpack<cpp_type>(inlined);
}

template struct LiteralDatatypeImpl<xsd_int,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Comparable,
                                    capabilities::Subtype,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
