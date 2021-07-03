#include "Quad.hpp"

namespace rdf4cpp::rdf {
Quad::Quad(Node subject, Node predicate, Node object) : QuadPattern(IRI::default_graph(), subject, predicate, object) {}
Quad::Quad(Node graph, Node subject, Node predicate, Node object) : QuadPattern(graph, subject, predicate, object) {}
bool Quad::valid() const {
    return ((graph().is_iri() or graph().is_bnode()) and not graph().null() and
            (subject().is_iri() or subject().is_bnode()) and not subject().null() and
            (predicate().is_iri()) and not predicate().null() and
            (object().is_iri() or object().is_literal() or object().is_bnode()) and not object().null());
}
std::optional<Quad> Quad::create_validated(Node graph, Node subject, Node predicate, Node object) {
    Quad quad{graph, subject, predicate, object};
    if (quad.valid())
        return quad;
    else
        return std::nullopt;
}
std::optional<Quad> Quad::create_validated(Node subject, Node predicate, Node object) {
    Quad quad{subject, predicate, object};
    if (quad.valid())
        return quad;
    else
        return std::nullopt;
}
}  // namespace rdf4cpp::rdf