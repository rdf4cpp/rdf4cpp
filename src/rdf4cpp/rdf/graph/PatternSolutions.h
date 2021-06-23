#ifndef RDF4CPP_PATTERNSOLUTIONS_H
#define RDF4CPP_PATTERNSOLUTIONS_H
#include <rdf4cpp/rdf/node/all.h>

#include <rdf4cpp/rdf/graph/Quad.h>
#include <rdf4cpp/rdf/graph/QuadPattern.h>

#include <rdf4cpp/rdf/graph/PatternSolution.h>


#include <iostream>
#include <set>
#include <utility>
namespace rdf4cpp::rdf::graph {


class PatternSolutions {
    using RDFNode = rdf4cpp::rdf::node::RDFNode;

    std::set<Quad> const *quads_;
    QuadPattern pattern_;

    mutable PatternSolution solution;
    std::set<Quad>::const_iterator iter_;
    std::set<Quad>::const_iterator end_;


public:
    PatternSolutions(const std::set<Quad> &quads, QuadPattern pattern);

    const QuadPattern &pattern();

    [[nodiscard]] size_t variable_count() const;

    PatternSolutions &begin();


    bool end();

    const PatternSolution &operator*() const;

    PatternSolutions &operator++();

    operator bool() const;

private:
    bool ended() const;

    void forward_to_solution();

    bool is_solution() const;

    // todo: as_string
};

}  // namespace rdf4cpp::rdf::graph

#endif  //RDF4CPP_PATTERNSOLUTIONS_H
