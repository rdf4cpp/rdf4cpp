#ifndef RDF4CPP_DATATYPEMAPPING_HPP
#define RDF4CPP_DATATYPEMAPPING_HPP

#include <rdf4cpp/rdf/datatypes/registry/ConstexprString.hpp>

#include <type_traits>

namespace rdf4cpp::rdf::datatypes::registry {

template<ConstexprString type_iri>
struct DatatypeMapping {
    using cpp_datatype = std::false_type;
};

}  // namespace rdf4cpp::rdf::datatypes::registry
#endif  //RDF4CPP_DATATYPEMAPPING_HPP
