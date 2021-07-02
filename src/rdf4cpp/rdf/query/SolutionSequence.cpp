#include "SolutionSequence.hpp"

#include <rdf4cpp/rdf/storage/tuple/DefaultSolutionSequence.hpp>

namespace rdf4cpp::rdf::query {
const QuadPattern &SolutionSequence::pattern() {
    return backend_->pattern();
}
size_t SolutionSequence::variable_count() const {
    return backend_->variable_count();
}
}  // namespace rdf4cpp::rdf::query