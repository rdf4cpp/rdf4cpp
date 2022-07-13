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
    using promoted = std::false_type;
};

template<ConstexprString type_iri>
struct DatatypeSupertypeMapping {
    using supertype = std::false_type;
};


namespace detail_rank {

template<ConstexprString type_iri, typename enable = void>
struct DatatypePromotionRank {
    static constexpr unsigned value = 0;
};

template<ConstexprString type_iri>
struct DatatypePromotionRank<type_iri, std::enable_if_t<!std::is_same_v<typename DatatypePromotionMapping<type_iri>::promoted, std::false_type>>> {
    static constexpr unsigned value = 1 + DatatypePromotionRank<DatatypePromotionMapping<type_iri>::promoted::identifier>::value;
};


template<ConstexprString type_iri, typename enable = void>
struct DatatypeSubtypeRank {
    static constexpr unsigned value = 0;
};

template<ConstexprString type_iri>
struct DatatypeSubtypeRank<type_iri, std::enable_if_t<!std::is_same_v<typename DatatypeSupertypeMapping<type_iri>::supertype, std::false_type>>> {
    static constexpr unsigned value = 1 + DatatypeSubtypeRank<DatatypeSupertypeMapping<type_iri>::supertype::identifier>::value;
};


} // namespace detail_rank
} // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEMAPPING_HPP
