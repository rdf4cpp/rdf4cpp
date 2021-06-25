#ifndef RDF4CPP_DATASET_H
#define RDF4CPP_DATASET_H

#include <rdf4cpp/rdf/graph/DefaultDatasetBackend.h>

#include "Graph.h"
#include <memory>
#include <utility>


namespace rdf4cpp::rdf::graph {

class Graph;

class Dataset {
    friend class Graph;
    using IRIResource = rdf4cpp::rdf::node::IRIResource;

    std::shared_ptr<DefaultDatasetBackend> dataset_backend_ = std::make_shared<DefaultDatasetBackend>();

    explicit Dataset(std::shared_ptr<DefaultDatasetBackend> datasetBackend);

public:
    Dataset() = default;

    void add(const Quad &quad);

    [[nodiscard]] bool contains(const Quad &quad) const;

    [[nodiscard]] PatternSolutions match(const QuadPattern &quad_pattern) const;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] size_t size(const node::IRIResource &graph_name) const;

    Graph graph(const node::IRIResource &graph_name);

    Graph graph();

    [[nodiscard]] std::string as_string() const;

    // TODO: iterator

    void add_ttl_file(const std::string &path);
};


}  // namespace rdf4cpp::rdf::graph
#endif  //RDF4CPP_DATASET_H
