#include "Dataset.hpp"

#include <rdf4cpp/rdf/Graph.hpp>
#include <utility>

namespace rdf4cpp::rdf {
Dataset::Dataset(Dataset::DatasetStorage dataset_storage) : dataset_storage(std::move(dataset_storage)) {}

Dataset::Dataset(Dataset::NodeStorage node_storage)
        : dataset_storage(Dataset::DatasetStorage::new_instance<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))) {}

void Dataset::add(const Quad &quad) {
    dataset_storage.add(quad);
}
bool Dataset::contains(const Quad &quad) const {
    return dataset_storage.contains(quad);
}
query::SolutionSequence Dataset::match(const query::QuadPattern &quad_pattern) const {
    return dataset_storage.match(quad_pattern);
}
size_t Dataset::size() const {
    return dataset_storage.size();
}
size_t Dataset::size(const IRI &graph_name) const {
    return dataset_storage.size(graph_name);
}
Graph Dataset::graph(const IRI &graph_name) {
    return Graph(dataset_storage, graph_name);
}
Graph Dataset::graph() {
    return Graph(dataset_storage, IRI::default_graph(dataset_storage.node_storage()));
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