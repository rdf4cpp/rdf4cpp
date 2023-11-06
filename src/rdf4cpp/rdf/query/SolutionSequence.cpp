#include <rdf4cpp/rdf/query/SolutionSequence.hpp>

#include <rdf4cpp/rdf/storage/tuple/DefaultSolutionSequenceBackend.hpp>

namespace rdf4cpp::rdf::query {
const QuadPattern &SolutionSequence::pattern() {
    return backend_->pattern();
}
size_t SolutionSequence::variable_count() const {
    return backend_->variable_count();
}
SolutionSequence::const_iterator SolutionSequence::begin() const {
    return backend_->begin();
}
SolutionSequence::const_iterator SolutionSequence::end() const {
    return backend_->end();
}
}  // namespace rdf4cpp::rdf::query