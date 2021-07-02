#include "SolutionSequenceBackend.hpp"
namespace rdf4cpp::rdf::storage::tuple {
SolutionSequenceBackend::SolutionSequenceBackend(SolutionSequenceBackend::QuadPattern pattern) : pattern_(pattern) {
}
SolutionSequenceBackend::~SolutionSequenceBackend() {}
const SolutionSequenceBackend::QuadPattern &SolutionSequenceBackend::pattern() {
    return pattern_;
}
}  // namespace rdf4cpp::rdf::storage::tuple