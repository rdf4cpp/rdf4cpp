#include "Dataset.hpp"

#include <rdf4cpp/rdf/Graph.hpp>
#include <utility>

namespace rdf4cpp::rdf {
Dataset::Dataset(Dataset::DatasetStorage dataset_storage) : dataset_storage(std::move(dataset_storage)) {}

Dataset::Dataset(Dataset::NodeStorage node_storage)
    : dataset_storage(Dataset::DatasetStorage::new_instance<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))) {}

void Dataset::add(const Quad &quad) {
    auto ns = backend().node_storage().upgrade();
    dataset_storage.add(quad.to_node_storage(ns));
}
bool Dataset::contains(const Quad &quad) const {
    auto ns = backend().node_storage().upgrade();
    return dataset_storage.contains(quad.to_node_storage(ns));
}
query::SolutionSequence Dataset::match(const query::QuadPattern &quad_pattern) const {
    auto ns = backend().node_storage().upgrade();
    return dataset_storage.match(quad_pattern.to_node_storage(ns));
}
size_t Dataset::size() const {
    return dataset_storage.size();
}
size_t Dataset::size(const IRI &graph_name) const {
    auto ns = backend().node_storage().upgrade();
    return dataset_storage.size(static_cast<IRI>(graph_name.to_node_storage(ns)));
}
Graph Dataset::graph(const IRI &graph_name) {
    return {dataset_storage, graph_name};
}
Graph Dataset::graph() {
    auto ns = backend().node_storage().upgrade();
    return {dataset_storage, IRI::default_graph(ns)};
}
Dataset::iterator Dataset::begin() const {
    return dataset_storage.begin();
}
Dataset::iterator Dataset::end() const {
    return dataset_storage.end();
}
Dataset::DatasetStorage &Dataset::backend() {
    return dataset_storage;
}
const Dataset::DatasetStorage &Dataset::backend() const {
    return dataset_storage;
}
}  // namespace rdf4cpp::rdf