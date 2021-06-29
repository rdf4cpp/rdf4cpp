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
    using NodeStorage = storage::node::NodeStorage;


    std::shared_ptr<IDatasetBackend> dataset_backend_;
    IRI graph_name = IRI::default_graph(dataset_backend_->node_storage());

    Graph(std::shared_ptr<IDatasetBackend> dataset_backend, const IRI &graph_name);

public:
    // TODO: allow to change default backend impl.
    explicit Graph(NodeStorage node_storage = NodeStorage::primary_instance());

    explicit Graph(const IRI &graph_name, NodeStorage node_storage = NodeStorage::primary_instance());

    void add(const Statement &statement);

    [[nodiscard]] bool contains(const Statement &statement) const;

    [[nodiscard]] query::PatternSolutions match(const query::TriplePattern &triple_pattern) const;

    [[nodiscard]] size_t size() const;

    Dataset dataset();

    [[nodiscard]] const IRI &name() const;

    std::shared_ptr<IDatasetBackend> &backend();

    [[nodiscard]] const std::shared_ptr<IDatasetBackend> &backend() const;

    // TODO: support union (+) and difference (-); open question: which graph name should be assigned?
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_GRAPH_HPP
