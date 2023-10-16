#include "DefaultSolutionSequenceBackend.hpp"
namespace rdf4cpp::rdf::storage::tuple {

DefaultSolutionSequenceBackend::DefaultSolutionSequenceBackend(DefaultSolutionSequenceBackend::QuadPattern pattern,
                                                               const dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad, dice::hash::Policies::wyhash>> *quads)
    : ISolutionSequenceBackend(pattern), quads_{quads} {
}
ISolutionSequenceBackend::const_iterator DefaultSolutionSequenceBackend::begin() const {
    return ISolutionSequenceBackend::const_iterator{const_iterator{quads_, pattern_}};
}
ISolutionSequenceBackend::const_iterator DefaultSolutionSequenceBackend::end() const {
    return ISolutionSequenceBackend::const_iterator{const_iterator{pattern_, quads_}};
}
DefaultSolutionSequenceBackend::const_iterator::const_iterator(const dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad, dice::hash::Policies::wyhash>> *data, const DefaultSolutionSequenceBackend::QuadPattern &pattern)
    : iter_(data->begin()), end_(data->end()), pattern_(pattern),
      solution_{pattern} {
    if (not ended())
        forward_to_solution();
}
DefaultSolutionSequenceBackend::const_iterator::const_iterator(const DefaultSolutionSequenceBackend::QuadPattern &pattern, const dice::sparse_map::sparse_set<Quad, dice::hash::DiceHash<Quad, dice::hash::Policies::wyhash>> *data)
    : iter_(data->end()), end_(data->end()), pattern_(pattern) {}
bool DefaultSolutionSequenceBackend::const_iterator::ended() const {
    return iter_ == end_;
}
void DefaultSolutionSequenceBackend::const_iterator::forward_to_solution() {
    while (!ended() && !is_solution()) {
        ++iter_;
    }
}
bool DefaultSolutionSequenceBackend::const_iterator::is_solution() const {
    auto pattern_it = pattern_.begin();
    size_t solution_pos = 0;
    for (const auto &entry : *iter_) {
        const auto &pattern_entry = *pattern_it;
        if (pattern_entry.is_variable()) {
            solution_[solution_pos++] = entry;
        } else {
            if (entry != pattern_entry)
                return false;
        }
        pattern_it++;
    }
    return true;
}
const DefaultSolutionSequenceBackend::const_iterator::value_type &DefaultSolutionSequenceBackend::const_iterator::operator*() const {
    return solution_;
}
DefaultSolutionSequenceBackend::const_iterator &DefaultSolutionSequenceBackend::const_iterator::operator++() {
    iter_++;
    forward_to_solution();
    return *this;
}
DefaultSolutionSequenceBackend::const_iterator DefaultSolutionSequenceBackend::const_iterator::operator++(int) & {
    auto copy = const_iterator(*this);
    iter_++;
    forward_to_solution();
    return copy;
}
bool DefaultSolutionSequenceBackend::const_iterator::operator==(const DefaultSolutionSequenceBackend::const_iterator &r) const {
    return this->iter_ == r.iter_;
}
bool DefaultSolutionSequenceBackend::const_iterator::operator!=(const DefaultSolutionSequenceBackend::const_iterator &r) const {
    return not(*this == r);
}
}  // namespace rdf4cpp::rdf::storage::tuple