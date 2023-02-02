#include "IDatasetBackend.hpp"

#include <utility>

namespace rdf4cpp::rdf::storage::tuple {

IDatasetBackend::IDatasetBackend(node::NodeStorage &node_storage) : node_storage_{node_storage.downgrade()} {}

IDatasetBackend::~IDatasetBackend() = default;
IDatasetBackend::const_iterator::const_iterator(const IDatasetBackend::const_iterator &r) : _impl(r._impl->clone()) {}
const IDatasetBackend::const_iterator::value_type &IDatasetBackend::const_iterator::operator*() const {
    return _impl->deref();
}
IDatasetBackend::const_iterator &IDatasetBackend::const_iterator::operator++() {
    _impl->next(1);
    return *this;
}
bool IDatasetBackend::const_iterator::operator==(const IDatasetBackend::const_iterator &r) const {
    return _impl->type() == r._impl->type() and _impl->equal(r._impl->address());
}
bool IDatasetBackend::const_iterator::operator!=(const IDatasetBackend::const_iterator &r) const {
    return not(*this == r);
}
}  // namespace rdf4cpp::rdf::storage::tuple