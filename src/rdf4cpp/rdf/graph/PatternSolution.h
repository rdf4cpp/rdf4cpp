#ifndef RDF4CPP_PATTERNSOLUTION_H
#define RDF4CPP_PATTERNSOLUTION_H


#include <rdf4cpp/rdf/node/all.h>

#include <rdf4cpp/rdf/graph/Quad.h>
#include <rdf4cpp/rdf/graph/QuadPattern.h>

namespace rdf4cpp::rdf::graph {

class PatternSolutions;

class PatternSolution {
    using RDFNode = rdf4cpp::rdf::node::RDFNode;
    using Variable = rdf4cpp::rdf::node::Variable;

    std::vector<Variable> variables;
    std::vector<RDFNode> current_solution;
    friend class PatternSolutions;

    static std::vector<Variable> extract_variables(const QuadPattern &quad);

public:
    PatternSolution() = default;

    explicit PatternSolution(std::vector<Variable> variables);
    explicit PatternSolution(const QuadPattern &quad_pattern);

    RDFNode operator[](const Variable &variable) const;

    RDFNode operator[](size_t pos) const;

    [[nodiscard]] Variable variable(size_t pos) const;

    [[nodiscard]] size_t variable_count() const;

    [[nodiscard]] size_t bound_count() const;

    // todo: as_string
    // todo: iterator
};
}  // namespace rdf4cpp::rdf::graph

#endif  //RDF4CPP_PATTERNSOLUTION_H
