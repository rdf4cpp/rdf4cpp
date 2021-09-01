#include "Dataset.hpp"

#include <rdf4cpp/rdf/Graph.hpp>
#include <utility>

namespace rdf4cpp::rdf {
Dataset::Dataset(Dataset::DatasetStorage dataset_storage) : dataset_storage(std::move(dataset_storage)) {}

Dataset::Dataset(Dataset::NodeStorage node_storage)
    : dataset_storage(Dataset::DatasetStorage::new_instance<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))) {}

void Dataset::add(const Quad &quad) {
    dataset_storage.add(quad.to_node_storage(backend().node_storage()));
}
bool Dataset::contains(const Quad &quad) const {
    return dataset_storage.contains(quad.to_node_storage(backend().node_storage()));
}
query::SolutionSequence Dataset::match(const query::QuadPattern &quad_pattern) const {
    return dataset_storage.match(quad_pattern.to_node_storage(backend().node_storage()));
}
size_t Dataset::size() const {
    return dataset_storage.size();
}
size_t Dataset::size(const IRI &graph_name) const {
    return dataset_storage.size((IRI) graph_name.to_node_storage(backend().node_storage()));
}
Graph Dataset::graph(const IRI &graph_name) {
    return {dataset_storage, graph_name};
}
Graph Dataset::graph() {
    return {dataset_storage, IRI::default_graph(backend().node_storage())};
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