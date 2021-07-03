#include "Statement.hpp"
namespace rdf4cpp::rdf {
Statement::Statement(Node subject, Node predicate, Node object) : query::TriplePattern(subject, predicate, object) {}
bool Statement::valid() const {
    return ((subject().is_iri() or subject().is_bnode()) and
            (predicate().is_iri()) and
            (object().is_iri() or object().is_literal() or object().is_bnode()));
}
}  // namespace rdf4cpp::rdf