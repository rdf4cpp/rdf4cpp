#include "LangString.hpp"

namespace rdf4cpp::datatypes::registry {

template struct LiteralDatatypeImpl<rdf_lang_string,
                                    capabilities::Comparable,
                                    capabilities::FixedId>;

}  // namespace rdf4cpp::datatypes::registry
