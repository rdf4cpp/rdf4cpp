#ifndef RDF4CPP_RDFNODETYPE_H
#define RDF4CPP_RDFNODETYPE_H

#include <cstdint>


namespace rdf4cpp::rdf::graph::node_manager {

enum class RDFNodeType : uint8_t {
    IRI = 0,
    BNode,
    Literal,
    Variable
};

}

#endif  //RDF4CPP_RDFNODETYPE_H
