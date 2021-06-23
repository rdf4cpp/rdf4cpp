#ifndef RDF4CPP_QUAD_H
#define RDF4CPP_QUAD_H

#include <rdf4cpp/rdf/graph/QuadPattern.h>

namespace rdf4cpp::rdf::graph {

class Quad : public QuadPattern {
public:
    Quad() = default;


    Quad(RDFNode graph, RDFNode subject, RDFNode predicate, RDFNode object);
    // TODO: make types more specific functions

    [[nodiscard]] bool valid() const;
};
}  // namespace rdf4cpp::rdf::graph

#endif  //RDF4CPP_QUAD_H
