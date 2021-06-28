#ifndef RDF4CPP_GRAPH_HPP
#define RDF4CPP_GRAPH_HPP

#include <rdf4cpp/rdf/Statement.hpp>
#include <rdf4cpp/rdf/query/TriplePattern.hpp>
#include <rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.hpp>
#include <rdf4cpp/rdf/storage/tuple/IDatasetBackend.hpp>

#include <memory>
#include <utility>

namespace rdf4cpp::rdf {

class Dataset;

class Graph {
    friend class Dataset;
    using IDatasetBackend = ::rdf4cpp::rdf::storage::tuple::IDatasetBackend;

    std::shared_ptr<IDatasetBackend> dataset_backend_ = std::make_shared<storage::tuple::DefaultDatasetBackend>();
    IRI graph_name = IRI::default_graph(dataset_backend_->node_storage());

    Graph(std::shared_ptr<IDatasetBackend> datasetBackend, const IRI &graphName);

public:
    Graph() = default;

    explicit Graph(const IRI &graphName);

    void add(const Statement &statement);

    [[nodiscard]] bool contains(const Statement &statement) const;

    [[nodiscard]] query::PatternSolutions match(const query::TriplePattern &triple_pattern) const;

    [[nodiscard]] size_t size() const;

    Dataset dataset();

    const IRI &name() const;

    std::shared_ptr<IDatasetBackend> &backend();

    [[nodiscard]] const std::shared_ptr<IDatasetBackend> &backend() const;
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_GRAPH_HPP
