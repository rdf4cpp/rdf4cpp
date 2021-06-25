#include "Quad.hpp"

namespace rdf4cpp::rdf {
Quad::Quad(Node graph, Node subject, Node predicate, Node object) : QuadPattern(graph, subject, predicate, object) {}
bool Quad::valid() const {
    return ((graph().is_iri() or graph().is_bnode()) and
            (subject().is_iri() or subject().is_bnode()) and
            (predicate().is_iri()) and
            (object().is_iri() or object().is_literal() or object().is_bnode()));
}
}  // namespace rdf4cpp::rdf