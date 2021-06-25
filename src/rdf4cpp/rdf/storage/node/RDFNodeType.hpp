#ifndef RDF4CPP_RDFNODETYPE_HPP
#define RDF4CPP_RDFNODETYPE_HPP

#include <cstdint>
#include <string>

namespace rdf4cpp::rdf::storage::node {

enum class RDFNodeType : uint8_t {
    Variable = 0,
    BNode,
    IRI,
    Literal
};

std::string as_string(RDFNodeType rdf_node_type);

}  // namespace rdf4cpp::rdf::storage::node


#endif  //RDF4CPP_RDFNODETYPE_HPP
