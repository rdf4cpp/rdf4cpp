#include "Graph.h"

#include <rdf4cpp/rdf/graph/Dataset.h>

namespace rdf4cpp::rdf::graph {

Graph::Graph(std::shared_ptr<DefaultDatasetBackend> datasetBackend, const Graph::IRIResource &graphName) : dataset_backend_(std::move(datasetBackend)), graph_name(graphName) {}

Graph::Graph(const Graph::IRIResource &graphName) : graph_name(graphName) {}

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

PatternSolutions Graph::match(const TriplePattern &triple_pattern) const {
    QuadPattern quad_pattern{graph_name,
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

}  // namespace rdf4cpp::rdf::graph