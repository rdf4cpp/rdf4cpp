#include "QuadPattern.hpp"
namespace rdf4cpp::rdf::query {
QuadPattern::operator std::string() const {

    return (graph().is_iri() and static_cast<IRI>(graph()).null())  // Graph
                    ? std::string{}
                    : static_cast<std::string>(graph()) + " " +
           static_cast<std::string>(subject()) + " " +    // Subject
           static_cast<std::string>(predicate()) + " " +  // Predicate
           static_cast<std::string>(object()) + " . ";    // Object
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
    return not(graph().null() or subject().null() or predicate().null() or object().null()) and
           ((graph().is_iri() or graph().is_variable()) and
            (not subject().is_literal()) and
            (predicate().is_iri() or predicate().is_variable()));
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
    os << static_cast<std::string>(pattern);
    return os;
}
QuadPattern QuadPattern::to_node_storage(storage::node::NodeStorage &node_storage) const {
    QuadPattern qp;
    auto it = qp.begin();
    for (const auto &item : (*this))
        if (item.backend_handle().node_storage_id() == node_storage.id())
            *(it++) = item;
        else
            *(it++) = item.to_node_storage(node_storage);
    return qp;
}
}  // namespace rdf4cpp::rdf::query