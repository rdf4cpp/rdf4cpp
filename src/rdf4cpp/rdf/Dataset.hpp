#ifndef RDF4CPP_DATASET_HPP
#define RDF4CPP_DATASET_HPP

#include <rdf4cpp/rdf/Graph.hpp>
#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/storage/tuple/DatasetStorage.hpp>
#include <rdf4cpp/rdf/storage/tuple/DefaultDatasetBackend.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>

#include <memory>
#include <utility>


namespace rdf4cpp::rdf {

class Graph;

class Dataset {
    using DatasetStorage = ::rdf4cpp::rdf::storage::tuple::DatasetStorage;
    using NodeStorage = storage::node::NodeStorage;
    friend class Graph;

    DatasetStorage dataset_storage;

    explicit Dataset(DatasetStorage dataset_storage);

public:
    template<typename BackendImpl, typename... Args>
    static inline Dataset new_instance(Args... args) {
        return {DatasetStorage::new_instance<BackendImpl>(args...)};
    }

    explicit Dataset(NodeStorage node_storage = NodeStorage::default_instance());

    void add(const Quad &quad);

    [[nodiscard]] bool contains(const Quad &quad) const;

    [[nodiscard]] query::SolutionSequence match(const query::QuadPattern &quad_pattern) const;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] size_t size(const IRI &graph_name) const;

    Graph graph(const IRI &graph_name);

    Graph graph();

    using const_interator = DatasetStorage::const_iterator;

    using iterator = const_interator;

    [[nodiscard]] iterator begin() const;

    [[nodiscard]] iterator end() const;

    DatasetStorage &backend();

    [[nodiscard]] const DatasetStorage &backend() const;

    /**
     * See Node::serialize for usage details
     */
    bool serialize(void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const noexcept;
    /**
     * See Node::serialize for usage details
     */
    bool serialize_trig(writer::SerializationState &state, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const noexcept;
    bool serialize_trig(void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const noexcept;

    /**
     * Serialize this dataset as <a href="https://www.w3.org/TR/n-quads/">N-Quads</a>.
     *
     * @param w a serializer
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    template<writer::BufWriter W>
    bool serialize(W &w) const noexcept {
        return serialize(&w.buffer(), &w.cursor(), &W::flush);
    }
    /**
     * Serialize this dataset as <a href="https://www.w3.org/TR/rdf12-trig/">TriG</a>.
     *
     * @param w a serializer
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    template<writer::BufWriter W>
    bool serialize_trig(writer::SerializationState &state, W &w) const noexcept {
        return serialize_trig(state, &w.buffer(), &w.cursor(), &W::flush);
    }

    template<writer::BufWriter W>
    bool serialize_trig(W &w) const noexcept {
        return serialize_trig(&w.buffer(), &w.cursor(), &W::flush);
    }

    friend std::ostream &operator<<(std::ostream &os, Dataset const &self);

    // TODO: support union (+) and difference (-)
    // TODO: add empty
};


}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_DATASET_HPP
