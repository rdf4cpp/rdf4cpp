#ifndef RDF4CPP_DATATYPEMAPPING_HPP
#define RDF4CPP_DATATYPEMAPPING_HPP

#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>

#include <type_traits>

namespace rdf4cpp::rdf::datatypes::registry {

template<ConstexprString type_iri>
struct DatatypeMapping {
    using cpp_datatype = std::false_type;
};

template<ConstexprString type_iri>
struct DatatypePromotionMapping {
    static constexpr ConstexprString<0> promoted_identifier{{}};
};

template<ConstexprString type_iri>
struct DatatypeSupertypeMapping {
    static constexpr ConstexprString<0> supertype_identifier{{}};
};


namespace detail_rank {

template<ConstexprString type_iri, typename enable = void>
struct DatatypePromotionRank {
    static constexpr unsigned value = 0;
};

template<ConstexprString type_iri>
struct DatatypePromotionRank<type_iri, std::enable_if_t<(DatatypePromotionMapping<type_iri>::promoted_identifier.size() > 0)>> {
    static constexpr unsigned value = 1 + DatatypePromotionRank<DatatypePromotionMapping<type_iri>::promoted_identifier>::value;
};


template<ConstexprString type_iri, typename enable = void>
struct DatatypeSubtypeRank {
    static constexpr unsigned value = 0;
};

template<ConstexprString type_iri>
struct DatatypeSubtypeRank<type_iri, std::enable_if_t<(DatatypeSupertypeMapping<type_iri>::supertype_identifier.size() > 0)>> {
    static constexpr unsigned value = 1 + DatatypeSubtypeRank<DatatypeSupertypeMapping<type_iri>::supertype_identifier>::value;
};


} // namespace detail::rank
} // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEMAPPING_HPP
