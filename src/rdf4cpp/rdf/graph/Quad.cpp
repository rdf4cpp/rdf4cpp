#include "Quad.h"
namespace rdf4cpp::rdf::graph {
Quad::Quad(QuadPattern::RDFNode graph, QuadPattern::RDFNode subject, QuadPattern::RDFNode predicate, QuadPattern::RDFNode object) : QuadPattern(graph, subject, predicate, object) {}
bool Quad::valid() const {
    return ((graph().is_iri() or graph().is_bnode()) and
            (subject().is_iri() or subject().is_bnode()) and
            (predicate().is_iri()) and
            (object().is_iri() or object().is_literal() or object().is_bnode()));
}
}  // namespace rdf4cpp::rdf::graph