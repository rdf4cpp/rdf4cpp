#ifndef RDF4CPP_REGISTRY_FIXED_ID_MAPPINGS_HPP
#define RDF4CPP_REGISTRY_FIXED_ID_MAPPINGS_HPP

#include <algorithm>
#include <string_view>

#include <rdf4cpp/rdf/datatypes/registry/util/ConstexprString.hpp>
#include <rdf4cpp/rdf/datatypes/registry/util/StaticFlatMap.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/LiteralType.hpp>

namespace rdf4cpp::rdf::datatypes::registry {

using storage::node::identifier::LiteralType;

inline constexpr util::StaticFlatMap<std::string_view, LiteralType, 63 /*2^6 - 1*/> reserved_datatype_ids{
        // { reserved for dynamic types,                           LiteralType::from_parts(false, 0) },
        { "",                                                      LiteralType::from_parts(false, 1) },
        { "http://www.w3.org/2001/XMLSchema#string",               LiteralType::from_parts(false, 2) },
        { "http://www.w3.org/1999/02/22-rdf-syntax-ns#langString", LiteralType::from_parts(false, 3) },

        { "http://www.w3.org/2001/XMLSchema#int",     LiteralType::from_parts(true, 0) },
        { "http://www.w3.org/2001/XMLSchema#integer", LiteralType::from_parts(true, 1) }};

inline constexpr uint8_t min_dynamic_datatype_id = reserved_datatype_ids.size() == 0
                                                           ? 1
                                                           : 1 + std::ranges::max_element(reserved_datatype_ids, {},
                                                                                          [](auto const &entry) {
                                                                                              return entry.second.to_underlying();
                                                                                          }) -> second.to_underlying();

} // rdf4cpp::rdf::datatypes::registry

#endif  //RDF4CPP_REGISTRY_FIXED_ID_MAPPINGS_HPP
