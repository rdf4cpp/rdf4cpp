#ifndef RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND
#define RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND

#include <concepts>
#include <rdf4cpp/Node.hpp>

namespace rdf4cpp::bnode_mngt {

template<typename G>
concept NodeGenerator = requires (G &g, storage::DynNodeStoragePtr node_storage) {
    /**
     * Generate a new node
     */
    { g.generate(node_storage) } -> std::convertible_to<Node>;
};


}  // namespace rdf4cpp::bnode_mngt

#endif  //RDF4CPP_RDF_UTIL_IBLANKNODEIDGENERATORBACKEND
