#include "Quad.h"
namespace rdf4cpp::rdf::graph {
Quad::Quad(QuadPattern::RDFNode graph, QuadPattern::RDFNode subject, QuadPattern::RDFNode predicate, QuadPattern::RDFNode object) : QuadPattern(graph, subject, predicate, object) {}
bool Quad::valid() const {
    return ((graph().is_iri()) and
            (subject().is_iri()) and
            (predicate().is_iri()) and
            (object().is_iri() or object().is_literal() or object().is_bnode()));
}
}  // namespace rdf4cpp::rdf::graph