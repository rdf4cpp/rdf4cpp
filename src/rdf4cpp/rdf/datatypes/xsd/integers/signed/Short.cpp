#include "Short.hpp"

#include <rdf4cpp/rdf/datatypes/registry/util/CharConvExt.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
capabilities::Default<xsd_short>::cpp_type capabilities::Default<xsd_short>::from_string(std::string_view s) {
    return util::from_chars<cpp_type, identifier>(s);
}

template<>
bool capabilities::Default<xsd_short>::serialize_canonical_string(cpp_type const &value, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) noexcept {
    return util::to_chars_canonical(value, buffer, cursor, flush);
}


template<>
bool capabilities::Logical<xsd_short>::effective_boolean_value(cpp_type const &value) noexcept {
    return value != 0;
}
#endif

template struct LiteralDatatypeImpl<xsd_short,
                                    capabilities::Logical,
                                    capabilities::NumericStub,
                                    capabilities::Subtype,
                                    capabilities::Comparable,
                                    capabilities::FixedId,
                                    capabilities::Inlineable>;

}  // namespace rdf4cpp::rdf::datatypes::registry
