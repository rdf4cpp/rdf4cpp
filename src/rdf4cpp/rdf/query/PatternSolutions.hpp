#ifndef RDF4CPP_PATTERNSOLUTIONS_HPP
#define RDF4CPP_PATTERNSOLUTIONS_HPP

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/PatternSolution.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>

#include <iostream>
#include <set>
#include <utility>

namespace rdf4cpp::rdf::query {


class PatternSolutions {

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

}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_PATTERNSOLUTIONS_HPP
