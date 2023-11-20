#ifndef RDF4CPP_GRAPH_HPP
#define RDF4CPP_GRAPH_HPP

#include <rdf4cpp/rdf/Statement.hpp>
#include <rdf4cpp/rdf/query/TriplePattern.hpp>
#include <rdf4cpp/rdf/storage/tuple/DatasetStorage.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>

#include <memory>
#include <utility>

namespace rdf4cpp::rdf {

class Dataset;

class Graph {
    friend class Dataset;
    using DatasetStorage = ::rdf4cpp::rdf::storage::tuple::DatasetStorage;
    using NodeStorage = storage::node::NodeStorage;

    DatasetStorage dataset_storage;
    IRI graph_name;

    Graph(DatasetStorage dataset_backend, const IRI &graph_name);

public:
    template<typename BackendImpl, typename... Args>
    static inline Graph new_instance(Args... args) {
        DatasetStorage dataset_backend = DatasetStorage::new_instance<BackendImpl>(args...);
        return {dataset_backend, IRI::default_graph(dataset_backend.node_storage())};
    }

    explicit Graph(NodeStorage node_storage = NodeStorage::default_instance());

    explicit Graph(const IRI &graph_name, NodeStorage node_storage = NodeStorage::default_instance());

    void add(const Statement &statement);

    [[nodiscard]] bool contains(const Statement &statement) const;

    [[nodiscard]] query::SolutionSequence match(const query::TriplePattern &triple_pattern) const;

    [[nodiscard]] size_t size() const;

    Dataset dataset();

    [[nodiscard]] const IRI &name() const;

    DatasetStorage &backend();

    [[nodiscard]] const DatasetStorage &backend() const;

    /**
     * See Node::serialize for usage details
     */
    bool serialize(void *buffer, writer::Cursor &cursor, writer::FlushFunc flush) const noexcept;

    /**
     * Serialize this graph as <a href="https://www.w3.org/TR/n-triples/">N-Triples</a>.
     *
     * @param w a serializer
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    template<writer::BufWriter W>
    bool serialize(W &w) const noexcept {
        return serialize(&w.buffer(), w.cursor(), &W::flush);
    }

    /**
     * Serialize this graph as <a href="https://www.w3.org/TR/n-triples/">N-Triples</a>.
     */
    friend std::ostream &operator<<(std::ostream &os, Graph const &graph);

    // TODO: support union (+) and difference (-); open question: which graph name should be assigned?
    // TODO: add empty
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_GRAPH_HPP
