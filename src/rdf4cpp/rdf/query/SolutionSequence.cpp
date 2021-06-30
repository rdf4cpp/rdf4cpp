#include "SolutionSequence.hpp"
namespace rdf4cpp::rdf::query {
SolutionSequence::SolutionSequence(const std::set<Quad> &quads, QuadPattern pattern)
    : quads_(&quads),
      pattern_(pattern),
      solution(this->pattern_),
      end_(this->quads_->cend()) {}
const QuadPattern &SolutionSequence::pattern() {
    return pattern_;
}
size_t SolutionSequence::variable_count() const {
    return solution.variable_count();
}
SolutionSequence &SolutionSequence::begin() {
    iter_ = quads_->begin();
    forward_to_solution();
    return *this;
}
bool SolutionSequence::end() {
    return false;
}
const Solution &SolutionSequence::operator*() const {
    return solution;
}
SolutionSequence &SolutionSequence::operator++() {
    iter_++;
    forward_to_solution();
    return *this;
}
SolutionSequence::operator bool() const {
    return not ended();
}
bool SolutionSequence::ended() const {
    return iter_ == end_;
}
void SolutionSequence::forward_to_solution() {
    while (not is_solution()) {
        if (not ended())
            iter_++;
        else
            break;
    }
}
bool SolutionSequence::is_solution() const {
    auto pattern_it = pattern_.begin();
    size_t solution_pos = 0;
    for (const auto &entry : *iter_) {
        const auto &pattern_entry = *pattern_it;
        if (pattern_entry.is_variable()) {
            solution.partial_mapping[solution_pos++].second = entry;
        } else {
            if (entry != pattern_entry)
                return false;
        }
        pattern_it++;
    }
    return true;
}
}  // namespace rdf4cpp::rdf::query