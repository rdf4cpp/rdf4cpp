#include "Graph.hpp"

#include <rdf4cpp/rdf/Dataset.hpp>

namespace rdf4cpp::rdf {

Graph::Graph(std::shared_ptr<storage::tuple::IDatasetBackend> datasetBackend, const IRI &graphName) : dataset_backend_(std::move(datasetBackend)), graph_name(graphName) {}

Graph::Graph(const IRI &graphName) : graph_name(graphName) {}

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

}  // namespace rdf4cpp::rdf