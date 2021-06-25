#ifndef RDF4CPP_DATASET_HPP
#define RDF4CPP_DATASET_HPP

#include <rdf4cpp/rdf/Graph.hpp>
#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.hpp>
#include <rdf4cpp/rdf/storage/tuple/IDatasetBackend.hpp>

#include <memory>
#include <utility>


namespace rdf4cpp::rdf {

class Graph;

class Dataset {
    using IDatasetBackend = ::rdf4cpp::rdf::storage::tuple::IDatasetBackend;
    friend class Graph;

    std::shared_ptr<IDatasetBackend> dataset_backend_ = std::make_shared<storage::tuple::DefaultDatasetBackend>();

    explicit Dataset(std::shared_ptr<IDatasetBackend> datasetBackend);

public:
    Dataset() = default;

    void add(const Quad &quad);

    [[nodiscard]] bool contains(const Quad &quad) const;

    [[nodiscard]] query::PatternSolutions match(const query::QuadPattern &quad_pattern) const;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] size_t size(const IRI &graph_name) const;

    Graph graph(const IRI &graph_name);

    Graph graph();

    [[nodiscard]] std::string as_string() const;

    // TODO: iterator

    void add_ttl_file(const std::string &path);
};


}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_DATASET_HPP
