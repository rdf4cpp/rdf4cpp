#include "TriplePattern.hpp"

namespace rdf4cpp::query {
TriplePattern::operator std::string() const {
    return static_cast<std::string>(subject()) + " " + static_cast<std::string>(predicate()) + " " + static_cast<std::string>(object()) + " . ";
}
TriplePattern::TriplePattern(Node subject, Node predicate, Node object) : entries_{{subject, predicate, object}} {}
Node &TriplePattern::subject() { return entries_[0]; }
const Node &TriplePattern::subject() const { return entries_[0]; }
Node &TriplePattern::predicate() { return entries_[1]; }
const Node &TriplePattern::predicate() const { return entries_[1]; }
Node &TriplePattern::object() { return entries_[2]; }
const Node &TriplePattern::object() const { return entries_[2]; }
bool TriplePattern::valid() const {
    return not(subject().null() or predicate().null() or object().null()) and
           (not subject().is_literal()) and
           (predicate().is_iri() or predicate().is_variable());
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
    os << static_cast<std::string>(pattern);
    return os;
}
TriplePattern TriplePattern::to_node_storage(storage::DynNodeStorage node_storage) const {
    TriplePattern tp;
    auto it = tp.begin();
    for (const auto &item : *this) {
        *(it++) = item.to_node_storage(node_storage);
    }

    return tp;
}
}  // namespace rdf4cpp::query