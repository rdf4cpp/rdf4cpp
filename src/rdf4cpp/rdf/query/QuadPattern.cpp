#include "QuadPattern.hpp"
namespace rdf4cpp::rdf::query {
QuadPattern::operator std::string() const {

    return (graph().is_iri() and ((IRI) graph()).identifier() == "")  // Graph
                   ? std::string{}
                   : (((std::string) graph()) + " ") +
                             (std::string) subject() + " " +  // Subject
                             (std::string) predicate() +      // Predicate
                             (std::string) object() + " . ";  // Object
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
std::ostream &operator<<(std::ostream &os, const QuadPattern &pattern) {
    os << (std::string) pattern;
    return os;
}
QuadPattern QuadPattern::to_node_storage(storage::node::NodeStorage &node_storage) const {
    QuadPattern qp;
    auto it = qp.begin();
    for (const auto &item : (*this))
        if (item.backend_handle().node_storage() == node_storage)
            *(it++) = item;
        else
            *(it++) = item.to_node_storage(node_storage);
    return qp;
}
}  // namespace rdf4cpp::rdf::query