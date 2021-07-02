#include "ISolutionSequenceBackend.hpp"
namespace rdf4cpp::rdf::storage::tuple {
ISolutionSequenceBackend::ISolutionSequenceBackend(ISolutionSequenceBackend::QuadPattern pattern) : pattern_(pattern) {
}
ISolutionSequenceBackend::~ISolutionSequenceBackend() {}
const ISolutionSequenceBackend::QuadPattern &ISolutionSequenceBackend::pattern() {
    return pattern_;
}
size_t ISolutionSequenceBackend::variable_count() const {
    return std::count_if(pattern_.begin(), pattern_.end(),
                         [](const Node &item) {
                             return not item.null() and item.is_variable();
                         });
}
const ISolutionSequenceBackend::const_iterator::value_type &ISolutionSequenceBackend::const_iterator::operator*() const {
    return _impl->deref();
}
ISolutionSequenceBackend::const_iterator &ISolutionSequenceBackend::const_iterator::operator++() {
    _impl->next(1);
    return *this;
}
bool ISolutionSequenceBackend::const_iterator::operator==(const ISolutionSequenceBackend::const_iterator &r) const {
    return _impl->type() == r._impl->type() and _impl->equal(r._impl->address());
}
bool ISolutionSequenceBackend::const_iterator::operator!=(const ISolutionSequenceBackend::const_iterator &r) const {
    return not(*this == r);
}
}  // namespace rdf4cpp::rdf::storage::tuple