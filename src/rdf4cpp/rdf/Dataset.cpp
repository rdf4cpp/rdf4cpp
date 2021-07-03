#include "Dataset.hpp"

#include <rdf4cpp/rdf/Graph.hpp>

namespace rdf4cpp::rdf {
Dataset::Dataset(std::shared_ptr<storage::tuple::IDatasetBackend> datasetBackend) : dataset_backend_(std::move(datasetBackend)) {}

Dataset::Dataset(Dataset::NodeStorage node_storage)
        : dataset_backend_(std::make_shared<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))) {}

void Dataset::add(const Quad &quad) {
    dataset_backend_->add(quad);
}
bool Dataset::contains(const Quad &quad) const {
    return dataset_backend_->contains(quad);
}
query::SolutionSequence Dataset::match(const query::QuadPattern &quad_pattern) const {
    return dataset_backend_->match(quad_pattern);
}
size_t Dataset::size() const {
    return dataset_backend_->size();
}
size_t Dataset::size(const IRI &graph_name) const {
    return dataset_backend_->size(graph_name);
}
Graph Dataset::graph(const IRI &graph_name) {
    return Graph(dataset_backend_, graph_name);
}
Graph Dataset::graph() {
    return Graph(dataset_backend_, IRI::default_graph(dataset_backend_->node_storage()));
}
Dataset::iterator Dataset::begin() const {
    return dataset_backend_->begin();
}
Dataset::iterator Dataset::end() const {
    return dataset_backend_->end();
}
std::shared_ptr<storage::tuple::IDatasetBackend> &Dataset::backend() {
    return dataset_backend_;
}
const std::shared_ptr<storage::tuple::IDatasetBackend> &Dataset::backend() const {
    return dataset_backend_;
}
}  // namespace rdf4cpp::rdf