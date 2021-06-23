#include "PatternSolutions.h"
namespace rdf4cpp::rdf::graph {
PatternSolutions::PatternSolutions(const std::set<Quad> &quads, QuadPattern pattern)
    : quads_(&quads),
      pattern_(pattern),
      solution(this->pattern_),
      end_(this->quads_->cend()) {}
const QuadPattern &PatternSolutions::pattern() {
    return pattern_;
}
size_t PatternSolutions::variable_count() const {
    return solution.variable_count();
}
PatternSolutions &PatternSolutions::begin() {
    iter_ = quads_->begin();
    forward_to_solution();
    return *this;
}
bool PatternSolutions::end() {
    return false;
}
const PatternSolution &PatternSolutions::operator*() const {
    return solution;
}
PatternSolutions &PatternSolutions::operator++() {
    iter_++;
    forward_to_solution();
    return *this;
}
PatternSolutions::operator bool() const {
    return not ended();
}
bool PatternSolutions::ended() const {
    return iter_ == end_;
}
void PatternSolutions::forward_to_solution() {
    while (not is_solution()) {
        if (not ended())
            iter_++;
        else
            break;
    }
}
bool PatternSolutions::is_solution() const {
    auto pattern_it = pattern_.begin();
    size_t solution_pos = 0;
    for (const auto &entry : *iter_) {
        const auto &pattern_entry = *pattern_it;
        if (pattern_entry.is_variable()) {
            solution.current_solution[solution_pos++] = entry;
        } else {
            if (entry != pattern_entry)
                return false;
        }
        pattern_it++;
    }
    return true;
}
}  // namespace rdf4cpp::rdf::graph