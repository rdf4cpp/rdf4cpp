#include "Dataset.h"
#include <rdf4cpp/rdf/graph/Graph.h>

namespace rdf4cpp::rdf::graph {
Dataset::Dataset(std::shared_ptr<DefaultDatasetBackend> datasetBackend) : dataset_backend_(std::move(datasetBackend)) {}
void Dataset::add(const Quad &quad) {
    dataset_backend_->add(quad);
}
bool Dataset::contains(const Quad &quad) const {
    return dataset_backend_->contains(quad);
}
PatternSolutions Dataset::match(const QuadPattern &quad_pattern) const {
    return dataset_backend_->match(quad_pattern);
}
size_t Dataset::size() const {
    return dataset_backend_->size();
}
size_t Dataset::size(const node::IRIResource &graph_name) const {
    return dataset_backend_->size(graph_name);
}
Graph Dataset::graph(const node::IRIResource &graph_name) {
    return Graph(dataset_backend_, graph_name);
}
Graph Dataset::graph() {
    return Graph(dataset_backend_, IRIResource::default_graph(dataset_backend_->node_storage()));
}
std::string Dataset::as_string() const {
    return dataset_backend_->as_string();
}
}  // namespace rdf4cpp::rdf::graph