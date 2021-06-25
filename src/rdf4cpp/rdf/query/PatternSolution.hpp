#ifndef RDF4CPP_PATTERNSOLUTION_HPP
#define RDF4CPP_PATTERNSOLUTION_HPP


#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>

namespace rdf4cpp::rdf::query {

class PatternSolutions;

class PatternSolution {

    std::vector<Variable> variables;
    std::vector<Node> current_solution;
    friend class PatternSolutions;

    static std::vector<Variable> extract_variables(const QuadPattern &quad);

public:
    PatternSolution() = default;

    explicit PatternSolution(std::vector<Variable> variables);
    explicit PatternSolution(const QuadPattern &quad_pattern);

    Node operator[](const Variable &variable) const;

    Node operator[](size_t pos) const;

    [[nodiscard]] Variable variable(size_t pos) const;

    [[nodiscard]] size_t variable_count() const;

    [[nodiscard]] size_t bound_count() const;

    // todo: as_string
    // todo: iterator
};
}  // namespace rdf4cpp::rdf::query

#endif  //RDF4CPP_PATTERNSOLUTION_HPP
