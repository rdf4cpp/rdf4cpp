#include "PatternSolution.h"
namespace rdf4cpp::rdf::graph {
std::vector<node::Variable> PatternSolution::extract_variables(const QuadPattern &quad) {
    std::vector<Variable> variables;
    for (const auto &entry : quad) {
        if (entry.is_variable())
            variables.push_back((Variable) entry);
    }
    return variables;
}
PatternSolution::PatternSolution(std::vector<Variable> variables) : variables(std::move(variables)), current_solution(this->variables.size()) {}
PatternSolution::PatternSolution(const QuadPattern &quad_pattern) : PatternSolution(extract_variables(quad_pattern)) {}
PatternSolution::RDFNode PatternSolution::operator[](const PatternSolution::Variable &variable) const {
    size_t pos = std::distance(variables.begin(), std::find(variables.begin(), variables.end(), variable));
    if (pos < variables.size()) {
        return current_solution[pos];
    } else {
        return {};
    }
}
PatternSolution::RDFNode PatternSolution::operator[](size_t pos) const {
    if (pos < current_solution.size())
        return current_solution[pos];
    else
        return {};
}
PatternSolution::Variable PatternSolution::variable(size_t pos) const {
    if (pos < variables.size())
        return variables[pos];
    else
        return {};
}
size_t PatternSolution::variable_count() const {
    return variables.size();
}
size_t PatternSolution::bound_count() const {
    return std::count_if(current_solution.begin(), current_solution.end(), [](RDFNode node) { return node != RDFNode{}; });
}
}  // namespace rdf4cpp::rdf::graph