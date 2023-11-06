#include <rdf4cpp/rdf/query/Solution.hpp>
namespace rdf4cpp::rdf::query {
std::vector<Variable> Solution::extract_variables(const QuadPattern &quad) {
    std::vector<Variable> variables;
    for (const auto &entry : quad) {
        if (entry.is_variable())
            variables.push_back(entry.as_variable());
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
const Node &Solution::operator[](size_t pos) const {
    assert(pos < partial_mapping.size());
    return partial_mapping[pos].second;
}

Node &Solution::operator[](size_t pos) {
    assert(pos < partial_mapping.size());
    return partial_mapping[pos].second;
}

const Variable &Solution::variable(size_t pos) const {
    assert(pos < partial_mapping.size());
    return partial_mapping[pos].first;
}
size_t Solution::variable_count() const {
    return partial_mapping.size();
}
size_t Solution::bound_count() const {
    return std::count_if(partial_mapping.begin(), partial_mapping.end(),
                         [](const auto &item) { return not item.second.null(); });
}

Solution::const_iterator Solution::end() const {
    return partial_mapping.end();
}
Solution::const_iterator Solution::begin() const {
    return partial_mapping.begin();
}
Solution::const_reverse_iterator Solution::rbegin() const {
    return partial_mapping.crbegin();
}
Solution::const_reverse_iterator Solution::rend() const {
    return partial_mapping.crend();
}
}  // namespace rdf4cpp::rdf::query