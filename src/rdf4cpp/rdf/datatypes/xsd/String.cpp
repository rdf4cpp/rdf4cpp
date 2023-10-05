#include <rdf4cpp/rdf/datatypes/xsd/String.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

#ifndef DOXYGEN_PARSER
template<>
bool capabilities::Logical<xsd_string>::effective_boolean_value(cpp_type const &value) noexcept {
    return !value.empty();
}
#endif

template struct LiteralDatatypeImpl<xsd_string,
                                    capabilities::Logical,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
