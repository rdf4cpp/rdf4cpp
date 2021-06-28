#include "TriplePattern.hpp"
namespace rdf4cpp::rdf::query {
TriplePattern::operator std::string() const {
    return (std::string) subject() + " " + (std::string) predicate() + (std::string) object() + " . ";
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
std::ostream &operator<<(std::ostream &os, const TriplePattern &pattern) {
    os << (std::string) pattern;
    return os;
}
}  // namespace rdf4cpp::rdf::query