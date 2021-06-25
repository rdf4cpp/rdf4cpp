#include "TriplePattern.hpp"
namespace rdf4cpp::rdf::query {
std::string TriplePattern::as_string() const {
    return subject().as_string(true) +
           " " + predicate().as_string(true) +
           " " + object().as_string(true) +
           " . ";
}
TriplePattern::TriplePattern(Node subject, Node predicate, Node object) : entries_{{subject, predicate, object}} {}
Node &TriplePattern::subject() { return entries_[0]; }
const Node &TriplePattern::subject() const { return entries_[0]; }
Node &TriplePattern::predicate() { return entries_[1]; }
const Node &TriplePattern::predicate() const { return entries_[1]; }
Node &TriplePattern::object() { return entries_[2]; }
const Node &TriplePattern::object() const { return entries_[2]; }
bool TriplePattern::valid() const {
    return ((subject().is_iri() or subject().is_variable()) and
            (predicate().is_iri() or predicate().is_variable()) and
            (object().is_iri() or object().is_literal() or object().is_variable()));
}
TriplePattern::iterator TriplePattern::begin() { return entries_.begin(); }
TriplePattern::const_iterator TriplePattern::begin() const { return entries_.begin(); }
TriplePattern::iterator TriplePattern::end() { return entries_.end(); }
TriplePattern::const_iterator TriplePattern::end() const { return entries_.end(); }
TriplePattern::reverse_iterator TriplePattern::rbegin() { return entries_.rbegin(); }
TriplePattern::const_reverse_iterator TriplePattern::rbegin() const { return entries_.rbegin(); }
TriplePattern::reverse_iterator TriplePattern::rend() { return entries_.rend(); }
TriplePattern::const_reverse_iterator TriplePattern::rend() const { return entries_.rend(); }
}  // namespace rdf4cpp::rdf::query