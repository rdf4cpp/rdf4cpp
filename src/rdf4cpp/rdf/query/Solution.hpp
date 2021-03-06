#ifndef RDF4CPP_SOLUTION_HPP
#define RDF4CPP_SOLUTION_HPP


#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>

namespace rdf4cpp::rdf::query {

class SolutionSequence;

class Solution {
    using Entries_t = std::vector<std::pair<Variable, Node>>;

    std::vector<std::pair<Variable, Node>> partial_mapping;
    friend class SolutionSequence;

    static std::vector<Variable> extract_variables(const QuadPattern &quad);

public:
    Solution() = default;

    explicit Solution(const std::vector<Variable> &variables);
    explicit Solution(const QuadPattern &quad_pattern);

    Node operator[](const Variable &variable) const;

    const Node &operator[](size_t pos) const;
    Node &operator[](size_t pos);

    [[nodiscard]] const Variable &variable(size_t pos) const;

    [[nodiscard]] size_t variable_count() const;

    [[nodiscard]] size_t bound_count() const;

    typedef typename Entries_t::const_iterator const_iterator;
    typedef const_iterator iterator;
    typedef typename Entries_t::const_reverse_iterator const_reverse_iterator;
    typedef const_reverse_iterator reverse_iterator;

    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] const_reverse_iterator rbegin() const;
    [[nodiscard]] const_reverse_iterator rend() const;
};
}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_SOLUTION_HPP
