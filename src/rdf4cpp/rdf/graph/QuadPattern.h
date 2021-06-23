#ifndef RDF4CPP_QUADPATTERN_H
#define RDF4CPP_QUADPATTERN_H

#include <array>
#include <rdf4cpp/rdf/node/IRIResource.h>
#include <rdf4cpp/rdf/node/RDFNode.h>


namespace rdf4cpp::rdf::graph {
class QuadPattern {
protected:
    using RDFNode = rdf4cpp::rdf::node::RDFNode;
    using IRIResource = rdf4cpp::rdf::node::IRIResource;
    using Variable = rdf4cpp::rdf::node::Variable;
    using Literal = rdf4cpp::rdf::node::Literal;

    using Entries_t = std::array<RDFNode, 4>;

    Entries_t entries_{};

public:
    QuadPattern() = default;
    QuadPattern(RDFNode graph, RDFNode subject, RDFNode predicate, RDFNode object);

    [[nodiscard]] RDFNode &graph();
    [[nodiscard]] const RDFNode &graph() const;

    [[nodiscard]] RDFNode &subject();
    [[nodiscard]] const RDFNode &subject() const;

    [[nodiscard]] RDFNode &predicate();
    [[nodiscard]] const RDFNode &predicate() const;

    [[nodiscard]] RDFNode &object();
    [[nodiscard]] const RDFNode &object() const;

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

    std::string as_string() const;
};
}  // namespace rdf4cpp::rdf::graph

#endif  //RDF4CPP_QUADPATTERN_H
