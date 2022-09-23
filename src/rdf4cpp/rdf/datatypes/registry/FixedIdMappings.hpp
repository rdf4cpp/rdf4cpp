#ifndef RDF4CPP_REGISTRY_FIXED_ID_MAPPINGS_HPP
#define RDF4CPP_REGISTRY_FIXED_ID_MAPPINGS_HPP

#include <algorithm>
#include <string_view>

#include <rdf4cpp/rdf/datatypes/registry/util/ConstexprString.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/StaticFlatMap.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/LiteralType.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

using storage::node::identifier::LiteralType;

inline constexpr util::ConstexprString default_graph_iri{""};
inline constexpr util::ConstexprString xsd_string{"http://www.w3.org/2001/XMLSchema#string"};
inline constexpr util::ConstexprString rdf_lang_string{"http://www.w3.org/1999/02/22-rdf-syntax-ns#langString"};
inline constexpr util::ConstexprString xsd_boolean{"http://www.w3.org/2001/XMLSchema#boolean"};

inline constexpr util::ConstexprString xsd_decimal{"http://www.w3.org/2001/XMLSchema#decimal"};
inline constexpr util::ConstexprString xsd_float{"http://www.w3.org/2001/XMLSchema#float"};
inline constexpr util::ConstexprString xsd_int{"http://www.w3.org/2001/XMLSchema#int"};
inline constexpr util::ConstexprString xsd_integer{"http://www.w3.org/2001/XMLSchema#integer"};

using ReservedDatatypeIdEntry = std::pair<std::string_view, LiteralType>;

/**
 * All datatypes that have their own LiteralTypes in the node storage and their own fixed id
 * in the registry are listed here.
 */
inline constexpr util::StaticFlatMap reserved_datatype_ids{
        // ReservedDatatypeIdEntry{reserved for dynamic types, LiteralType::from_parts(false, 0)},
        ReservedDatatypeIdEntry{default_graph_iri, LiteralType::from_parts(false, 1)},
        ReservedDatatypeIdEntry{xsd_string,        LiteralType::from_parts(false, 2)},
        ReservedDatatypeIdEntry{rdf_lang_string,   LiteralType::from_parts(false, 3)},
        ReservedDatatypeIdEntry{xsd_boolean,       LiteralType::from_parts(false, 4)},

        ReservedDatatypeIdEntry{xsd_float,   LiteralType::from_parts(true, 0)},
        // ReservedDatatypeIdEntry{xsd_double,  LiteralType::from_parts(true, 1)},
        ReservedDatatypeIdEntry{xsd_decimal, LiteralType::from_parts(true, 2)},

        ReservedDatatypeIdEntry{xsd_integer,              LiteralType::from_parts(true, 3)},
        // ReservedDatatypeIdEntry{xsd_non_positive_integer, LiteralType::from_parts(true, 4)},
        // ReservedDatatypeIdEntry{xsd_long,                 LiteralType::from_parts(true, 5)},
        // ReservedDatatypeIdEntry{xsd_non_negative_integer, LiteralType::from_parts(true, 6)},
        // ReservedDatatypeIdEntry{xsd_negative_integer,     LiteralType::from_parts(true, 7)},
        ReservedDatatypeIdEntry{xsd_int,                  LiteralType::from_parts(true, 8)}
        // ReservedDatatypeIdEntry{xsd_unsigned_long,        LiteralType::from_parts(true, 9)},
        // ReservedDatatypeIdEntry{xsd_positive_integer,     LiteralType::from_parts(true, 10)},
        // ReservedDatatypeIdEntry{xsd_short,                LiteralType::from_parts(true, 11)},
        // ReservedDatatypeIdEntry{xsd_unsigned_int,         LiteralType::from_parts(true, 12)},
        // ReservedDatatypeIdEntry{xsd_byte,                 LiteralType::from_parts(true, 13)},
        // ReservedDatatypeIdEntry{xsd_unsigned_short,       LiteralType::from_parts(true, 14)},
        // ReservedDatatypeIdEntry{xsd_unsigned_byte,        LiteralType::from_parts(true, 15)}
};

/**
 * the first id that can be used for dynamic datatypes
 * this is used for two things:
 *      - determine the first iri id that can be dynamically assigned
 *      - determine the size of the static (constant-time-lookup) section of the registry
 */
inline constexpr uint64_t min_dynamic_datatype_id = reserved_datatype_ids.size() == 0
                                                            ? 1
                                                            : 1 + std::ranges::max_element(reserved_datatype_ids, {},
                                                                                           [](auto const &entry) {
                                                                                               return entry.second.to_underlying();
                                                                                           })->second.to_underlying();

} // rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_REGISTRY_FIXED_ID_MAPPINGS_HPP
