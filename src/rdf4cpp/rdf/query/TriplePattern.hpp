#ifndef RDF4CPP_TRIPLEPATTERN_HPP
#define RDF4CPP_TRIPLEPATTERN_HPP

#include <rdf4cpp/rdf/Node.hpp>

#include <array>
#include <ostream>

namespace rdf4cpp::rdf::query {
/**
 * <div>TriplePattern</div> is modeled around SPARQL Triple patterns.
 *
 * One important difference is that pattern matching with <div>BlankNode</div>s is allowed for subject and object.
 * For the equivalent of a SPARQL anonymous variables, use Variable and set anonymous to true.
 *
 * @see <https://www.w3.org/TR/2013/REC-sparql11-update-20130321/#def_datasetQuadPattern>
 */
class TriplePattern {
    // TODO: adjust API to Quad
protected:
    using Variable = rdf4cpp::rdf::query::Variable;

    using Entries_t = std::array<Node, 3>;

    Entries_t entries_;

public:
    TriplePattern() = default;

    TriplePattern(Node subject, Node predicate, Node object);

    [[nodiscard]] Node &subject();
    [[nodiscard]] const Node &subject() const;

    [[nodiscard]] Node &predicate();
    [[nodiscard]] const Node &predicate() const;

    [[nodiscard]] Node &object();
    [[nodiscard]] const Node &object() const;

    [[nodiscard]] bool valid() const;

    typedef typename Entries_t::iterator iterator;
    typedef typename Entries_t::const_iterator const_iterator;
    typedef typename Entries_t::reverse_iterator reverse_iterator;
    typedef typename Entries_t::const_reverse_iterator const_reverse_iterator;

    [[nodiscard]] iterator begin();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] reverse_iterator rbegin();
    [[nodiscard]] const_reverse_iterator rbegin() const;
    [[nodiscard]] reverse_iterator rend();
    [[nodiscard]] const_reverse_iterator rend() const;

    auto operator<=>(const TriplePattern &rhs) const = default;

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const TriplePattern &pattern);

    [[nodiscard]] TriplePattern to_node_storage(storage::node::NodeStorage &node_storage) const;
};
}  // namespace rdf4cpp::rdf::query


#endif  //RDF4CPP_TRIPLEPATTERN_HPP
