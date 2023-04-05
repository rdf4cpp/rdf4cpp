#include <rdf4cpp/rdf/datatypes/rdf/LangString.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

template struct LiteralDatatypeImpl<rdf_lang_string,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::rdf::datatypes::registry
