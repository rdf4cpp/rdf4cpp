#include "Graph.hpp"

#include <rdf4cpp/rdf/Dataset.hpp>

#include <utility>

namespace rdf4cpp::rdf {

Graph::Graph(storage::tuple::DatasetStorage dataset_storage, const IRI &graph_name)
    : dataset_storage(std::move(dataset_storage)),
      graph_name(graph_name.to_node_storage(backend().node_storage())) {}

Graph::Graph(storage::node::NodeStorage node_storage)
    : dataset_storage(Dataset::DatasetStorage::new_instance<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))),
      graph_name(IRI::default_graph(this->dataset_storage.node_storage())) {}

Graph::Graph(const IRI &graph_name, NodeStorage node_storage) : Graph(std::move(node_storage)) {
    this->graph_name = (IRI) graph_name.to_node_storage(backend().node_storage());
}

void Graph::add(const Statement &statement) {
    Quad quad{graph_name,
              statement.subject().to_node_storage(backend().node_storage()),
              statement.predicate().to_node_storage(backend().node_storage()),
              statement.object().to_node_storage(backend().node_storage())};
    dataset_storage.add(quad);
}

bool Graph::contains(const Statement &statement) const {
    Quad quad{graph_name,
              statement.subject().to_node_storage(backend().node_storage()),
              statement.predicate().to_node_storage(backend().node_storage()),
              statement.object().to_node_storage(backend().node_storage())};
    return dataset_storage.contains(quad);
}

query::SolutionSequence Graph::match(const query::TriplePattern &triple_pattern) const {
    query::QuadPattern quad_pattern{graph_name,
                                    triple_pattern.subject().to_node_storage(backend().node_storage()),
                                    triple_pattern.predicate().to_node_storage(backend().node_storage()),
                                    triple_pattern.object().to_node_storage(backend().node_storage())};
    return dataset_storage.match(quad_pattern);
}

size_t Graph::size() const {
    return dataset_storage.size(graph_name);
}
Dataset Graph::dataset() {
    return Dataset(dataset_storage);
}
const IRI &Graph::name() const {
    return graph_name;
}
storage::tuple::DatasetStorage &Graph::backend() {
    return dataset_storage;
}
const storage::tuple::DatasetStorage &Graph::backend() const {
    return dataset_storage;
}

}  // namespace rdf4cpp::rdf