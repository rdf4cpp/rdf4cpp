#include "DatasetStorage.hpp"
#include <rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.hpp>


namespace rdf4cpp::rdf::storage::tuple {

DatasetStorage::DatasetStorage(std::shared_ptr<IDatasetBackend> dataset_backend) : backend_(std::move(dataset_backend)) {}

DatasetStorage DatasetStorage::default_instance_ = new_instance();

DatasetStorage &DatasetStorage::primary_instance() {
    return DatasetStorage::default_instance_;
}

DatasetStorage DatasetStorage::new_instance() {
    return DatasetStorage(std::make_shared<DefaultDatasetBackend>());
}

void DatasetStorage::add(const Quad &quad) {
    backend_->add(quad);
}
bool DatasetStorage::contains(const Quad &quad) const {
    return backend_->contains(quad);
}
size_t DatasetStorage::size() const {
    return backend_->size();
}
DatasetStorage::PatternSolutions DatasetStorage::match(const DatasetStorage::QuadPattern &quad_pattern) const {
    return backend_->match(quad_pattern);
}
size_t DatasetStorage::size(const IRI &graph_name) const {
    return backend_->size(graph_name);
}
DatasetStorage::const_iterator DatasetStorage::begin() const {
    return backend_->begin();
}
DatasetStorage::const_iterator DatasetStorage::end() const {
    return backend_->end();
}
node::NodeStorage &DatasetStorage::node_storage() const {
    return backend_->node_storage();
}
const std::shared_ptr<IDatasetBackend> &DatasetStorage::backend_impl() const {
    return backend_;
}

std::shared_ptr<IDatasetBackend> &DatasetStorage::backend_impl() {
    return backend_;
}

}  // namespace rdf4cpp::rdf::storage::tuple