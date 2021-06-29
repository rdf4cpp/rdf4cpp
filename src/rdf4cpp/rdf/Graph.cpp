#include "Graph.hpp"

#include <rdf4cpp/rdf/Dataset.hpp>

#include <utility>

namespace rdf4cpp::rdf {

Graph::Graph(std::shared_ptr<storage::tuple::IDatasetBackend> dataset_backend, const IRI &graph_name) : dataset_backend_(std::move(dataset_backend)), graph_name(graph_name) {}

Graph::Graph(Graph::NodeStorage node_storage)
    : dataset_backend_(std::make_shared<storage::tuple::DefaultDatasetBackend>(std::move(node_storage))) {}

Graph::Graph(const IRI &graph_name, NodeStorage node_storage) : Graph(std::move(node_storage)) {
    this->graph_name = graph_name;
}

void Graph::add(const Statement &statement) {
    Quad quad{graph_name,
              statement.subject(),
              statement.predicate(),
              statement.object()};
    dataset_backend_->add(quad);
}

bool Graph::contains(const Statement &statement) const {
    Quad quad{graph_name,
              statement.subject(),
              statement.predicate(),
              statement.object()};
    return dataset_backend_->contains(quad);
}

query::PatternSolutions Graph::match(const query::TriplePattern &triple_pattern) const {
    query::QuadPattern quad_pattern{graph_name,
                                    triple_pattern.subject(),
                                    triple_pattern.predicate(),
                                    triple_pattern.object()};
    return dataset_backend_->match(quad_pattern);
}

size_t Graph::size() const {
    return dataset_backend_->size(graph_name);
}
Dataset Graph::dataset() {
    return Dataset(dataset_backend_);
}
const IRI &Graph::name() const {
    return graph_name;
}
std::shared_ptr<storage::tuple::IDatasetBackend> &Graph::backend() {
    return dataset_backend_;
}
const std::shared_ptr<storage::tuple::IDatasetBackend> &Graph::backend() const {
    return dataset_backend_;
}

}  // namespace rdf4cpp::rdf