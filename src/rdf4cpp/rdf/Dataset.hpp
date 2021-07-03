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
    using NodeStorage = storage::node::NodeStorage;
    friend class Graph;

    std::shared_ptr<IDatasetBackend> dataset_backend_;

    explicit Dataset(std::shared_ptr<IDatasetBackend> datasetBackend);

public:
    // TODO: allow to change default backend impl.
    explicit Dataset(NodeStorage node_storage = NodeStorage::primary_instance());


    void add(const Quad &quad);

    [[nodiscard]] bool contains(const Quad &quad) const;

    [[nodiscard]] query::SolutionSequence match(const query::QuadPattern &quad_pattern) const;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] size_t size(const IRI &graph_name) const;

    Graph graph(const IRI &graph_name);

    Graph graph();

    /**
     * Load turtle file into default graph.
     * TODO: this is provisional until a native parser is implemented
     * @param path path to ttl file
     */
    void add_ttl_file(const std::string &path);

    using const_interator = IDatasetBackend::const_iterator;

    using iterator = const_interator;

    [[nodiscard]] iterator begin() const;

    [[nodiscard]] iterator end() const;

    std::shared_ptr<IDatasetBackend> &backend();

    [[nodiscard]] const std::shared_ptr<IDatasetBackend> &backend() const;

    // TODO: support union (+) and difference (-)
    // TODO: add size
    // TODO: add empty
};


}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_DATASET_HPP
