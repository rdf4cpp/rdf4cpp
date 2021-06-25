#ifndef RDF4CPP_QUAD_HPP
#define RDF4CPP_QUAD_HPP

#include <rdf4cpp/rdf/query/QuadPattern.hpp>

namespace rdf4cpp::rdf {

class Quad : public query::QuadPattern {
public:
    Quad() = default;


    Quad(Node graph, Node subject, Node predicate, Node object);
    // TODO: make types more specific functions

    [[nodiscard]] bool valid() const;
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_QUAD_HPP
