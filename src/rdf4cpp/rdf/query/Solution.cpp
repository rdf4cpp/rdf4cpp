#include "Solution.hpp"
namespace rdf4cpp::rdf::query {
std::vector<Variable> Solution::extract_variables(const QuadPattern &quad) {
    std::vector<Variable> variables;
    for (const auto &entry : quad) {
        if (entry.is_variable())
            variables.push_back((Variable) entry);
    }
    return variables;
}

std::vector<std::pair<Variable, Node>> init(const std::vector<Variable> &variables) {
    std::vector<std::pair<Variable, Node>> partial_mapping;
    for (const auto &var_name : variables)
        partial_mapping.emplace_back(var_name, Node());
    return partial_mapping;
}

Solution::Solution(const std::vector<Variable> &variables) : partial_mapping(init(variables)) {}
Solution::Solution(const QuadPattern &quad_pattern) : Solution(extract_variables(quad_pattern)) {}
Node Solution::operator[](const Variable &variable) const {
    size_t pos = std::distance(partial_mapping.begin(), std::find_if(partial_mapping.begin(), partial_mapping.end(),
                                                                     [=](const auto &item) { return item.first == variable; }));
    if (pos < partial_mapping.size()) {
        return partial_mapping[pos].second;
    } else {
        return {};
    }
}
Node Solution::operator[](size_t pos) const {
    if (pos < partial_mapping.size())
        return partial_mapping[pos].second;
    else
        return {};
}
Variable Solution::variable(size_t pos) const {
    if (pos < partial_mapping.size())
        return partial_mapping[pos].first;
    else
        return {};
}
size_t Solution::variable_count() const {
    return partial_mapping.size();
}
size_t Solution::bound_count() const {
    return std::count_if(partial_mapping.begin(), partial_mapping.end(),
                         [](const auto &item) { return not item.second.null(); });
}
}  // namespace rdf4cpp::rdf::query