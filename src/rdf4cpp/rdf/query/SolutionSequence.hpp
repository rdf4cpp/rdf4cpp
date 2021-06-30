#ifndef RDF4CPP_SOLUTIONSEQUENCE_HPP
#define RDF4CPP_SOLUTIONSEQUENCE_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>
#include <rdf4cpp/rdf/query/Solution.hpp>

#include <iostream>
#include <set>
#include <utility>

namespace rdf4cpp::rdf::query {


class SolutionSequence {
    // TODO: make pure virtual and add Iterator to DefaultDatasetBackend
    std::set<Quad> const *quads_;
    QuadPattern pattern_;

    mutable Solution solution;
    std::set<Quad>::const_iterator iter_;
    std::set<Quad>::const_iterator end_;


public:
    SolutionSequence(const std::set<Quad> &quads, QuadPattern pattern);

    const QuadPattern &pattern();

    [[nodiscard]] size_t variable_count() const;

    SolutionSequence &begin();


    bool end();

    const Solution &operator*() const;

    SolutionSequence &operator++();

    operator bool() const;

private:
    bool ended() const;

    void forward_to_solution();

    bool is_solution() const;
};

}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_SOLUTIONSEQUENCE_HPP
