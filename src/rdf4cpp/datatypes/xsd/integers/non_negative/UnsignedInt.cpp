#include "UnsignedInt.hpp"

#include <rdf4cpp/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_unsigned_int>::cpp_type capabilities::Default<xsd_unsigned_int>::from_string(std::string_view s) {
    return util::from_chars<cpp_type, identifier>(s);
}

template<>
bool capabilities::Default<xsd_unsigned_int>::serialize_canonical_string(cpp_type const &value, writer::BufWriterParts writer) noexcept {
    return util::to_chars_canonical(value, writer);
}

template<>
bool capabilities::Logical<xsd_unsigned_int>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}
#endif

template struct LiteralDatatypeImpl<xsd_unsigned_int,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Comparable,
                                    capabilities::Subtype,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::datatypes::registry
