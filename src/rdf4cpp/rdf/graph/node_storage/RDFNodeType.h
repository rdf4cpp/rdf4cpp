#ifndef RDF4CPP_RDFNODETYPE_H
#define RDF4CPP_RDFNODETYPE_H

#include <cstdint>
#include <string>

namespace rdf4cpp::rdf::graph::node_storage {

enum class RDFNodeType : uint8_t {
    Variable = 0,
    BNode,
    IRI,
    Literal
};

std::string as_string(RDFNodeType rdf_node_type);

}  // namespace rdf4cpp::rdf::graph::node_storage


#endif  //RDF4CPP_RDFNODETYPE_H
