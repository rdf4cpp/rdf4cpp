#ifndef RDF4CPP_SOLUTION_HPP
#define RDF4CPP_SOLUTION_HPP


#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>

namespace rdf4cpp::rdf::query {

class SolutionSequence;

class Solution {

    std::vector<std::pair<Variable, Node>> partial_mapping;
    friend class SolutionSequence;

    static std::vector<Variable> extract_variables(const QuadPattern &quad);

public:
    Solution() = default;

    explicit Solution(const std::vector<Variable> &variables);
    explicit Solution(const QuadPattern &quad_pattern);

    Node operator[](const Variable &variable) const;

    Node operator[](size_t pos) const;

    [[nodiscard]] Variable variable(size_t pos) const;

    [[nodiscard]] size_t variable_count() const;

    [[nodiscard]] size_t bound_count() const;

    // todo: iterator
};
}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_SOLUTION_HPP
