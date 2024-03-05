#ifndef RDF4CPP_QUADPATTERN_HPP
#define RDF4CPP_QUADPATTERN_HPP

#include <rdf4cpp/Node.hpp>

#include <array>
#include <ostream>

namespace rdf4cpp::rdf::query {
/**
 * <div>QuadPattern</div> is modeled around SPARQL qual patterns.
 *
 * One important difference is that pattern matching with <div>BlankNode</div>s is allowed subject and object.
 * For the equivalent of SPARQL anonymous variables, use Variable and set anonymous to true.
 *
 * @see <https://www.w3.org/TR/sparql11-query/#defn_TriplePattern>
 */
class QuadPattern {
    // TODO: adjust API to Quad
protected:
    using Entries_t = std::array<Node, 4>;

    Entries_t entries_{};

public:
    QuadPattern() = default;
    QuadPattern(Node graph, Node subject, Node predicate, Node object);

    [[nodiscard]] Node &graph();
    [[nodiscard]] const Node &graph() const;

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

    auto operator<=>(const QuadPattern &rhs) const = default;

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const QuadPattern &pattern);

    [[nodiscard]] QuadPattern to_node_storage(storage::DynNodeStorage node_storage) const;
};
}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_QUADPATTERN_HPP
