#ifndef RDF4CPP_GRAPH_H
#define RDF4CPP_GRAPH_H

#include <rdf4cpp/rdf/graph/DefaultDatasetBackend.h>
#include <rdf4cpp/rdf/graph/Statement.h>
#include <rdf4cpp/rdf/graph/TriplePattern.h>

#include <rdf4cpp/rdf/node/all.h>

#include <memory>
#include <utility>

namespace rdf4cpp::rdf::graph {

class Dataset;

class Graph {
    friend class Dataset;
    using RDFNode = rdf4cpp::rdf::node::RDFNode;
    using IRIResource = rdf4cpp::rdf::node::IRIResource;
    using Variable = rdf4cpp::rdf::node::Variable;
    using Literal = rdf4cpp::rdf::node::Literal;

    std::shared_ptr<DefaultDatasetBackend> dataset_backend_ = std::make_shared<DefaultDatasetBackend>();
    IRIResource graph_name = IRIResource::default_graph(dataset_backend_->node_storage());

    Graph(std::shared_ptr<DefaultDatasetBackend> datasetBackend, const IRIResource &graphName);

public:
    Graph() = default;

    explicit Graph(const IRIResource &graphName);

    void add(const Statement &statement);

    [[nodiscard]] bool contains(const Statement &statement) const;

    [[nodiscard]] PatternSolutions match(const TriplePattern &triple_pattern) const;

    [[nodiscard]] size_t size() const;

    Dataset dataset();

    [[nodiscard]] std::string as_string() const;

    // TODO: iterator

    // TODO: to string
};
}  // namespace rdf4cpp::rdf::graph

#endif  //RDF4CPP_GRAPH_H
