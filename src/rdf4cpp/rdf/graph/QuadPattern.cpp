#include "QuadPattern.h"
namespace rdf4cpp::rdf::graph {
std::string QuadPattern::as_string() const {
    std::string str = (graph().is_iri() and ((IRIResource) graph()).as_string() == "") ? "" : (graph().as_string(true) + " ");
    str += subject().as_string(true) +
           " " + predicate().as_string(true) +
           " " + object().as_string(true) +
           " . ";
    return str;
}
QuadPattern::QuadPattern(QuadPattern::RDFNode graph, QuadPattern::RDFNode subject, QuadPattern::RDFNode predicate, QuadPattern::RDFNode object) : entries_({graph, subject, predicate, object}) {}
QuadPattern::RDFNode &QuadPattern::graph() { return entries_[0]; }
const QuadPattern::RDFNode &QuadPattern::graph() const { return entries_[0]; }
QuadPattern::RDFNode &QuadPattern::subject() { return entries_[1]; }
const QuadPattern::RDFNode &QuadPattern::subject() const { return entries_[1]; }
QuadPattern::RDFNode &QuadPattern::predicate() { return entries_[2]; }
const QuadPattern::RDFNode &QuadPattern::predicate() const { return entries_[2]; }
QuadPattern::RDFNode &QuadPattern::object() { return entries_[3]; }
const QuadPattern::RDFNode &QuadPattern::object() const { return entries_[3]; }
bool QuadPattern::valid() const {
    return ((graph().is_iri() or graph().is_variable()) and
            (subject().is_iri() or subject().is_variable()) and
            (predicate().is_iri() or predicate().is_variable()) and
            (object().is_iri() or object().is_literal() or object().is_variable()));
}
QuadPattern::iterator QuadPattern::begin() { return entries_.begin(); }
QuadPattern::const_iterator QuadPattern::begin() const { return entries_.begin(); }
QuadPattern::iterator QuadPattern::end() { return entries_.end(); }
QuadPattern::const_iterator QuadPattern::end() const { return entries_.end(); }
QuadPattern::reverse_iterator QuadPattern::rbegin() { return entries_.rbegin(); }
QuadPattern::const_reverse_iterator QuadPattern::rbegin() const { return entries_.rbegin(); }
QuadPattern::reverse_iterator QuadPattern::rend() { return entries_.rend(); }
QuadPattern::const_reverse_iterator QuadPattern::rend() const { return entries_.rend(); }
}  // namespace rdf4cpp::rdf::graph