#include "QuadPattern.hpp"
namespace rdf4cpp::rdf::query {
std::string QuadPattern::as_string() const {
    std::string str = (graph().is_iri() and ((IRI) graph()).as_string() == "") ? "" : (graph().as_string(true) + " ");
    str += subject().as_string(true) +
           " " + predicate().as_string(true) +
           " " + object().as_string(true) +
           " . ";
    return str;
}
QuadPattern::QuadPattern(Node graph, Node subject, Node predicate, Node object) : entries_({graph, subject, predicate, object}) {}
Node &QuadPattern::graph() { return entries_[0]; }
const Node &QuadPattern::graph() const { return entries_[0]; }
Node &QuadPattern::subject() { return entries_[1]; }
const Node &QuadPattern::subject() const { return entries_[1]; }
Node &QuadPattern::predicate() { return entries_[2]; }
const Node &QuadPattern::predicate() const { return entries_[2]; }
Node &QuadPattern::object() { return entries_[3]; }
const Node &QuadPattern::object() const { return entries_[3]; }
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
}  // namespace rdf4cpp::rdf::query