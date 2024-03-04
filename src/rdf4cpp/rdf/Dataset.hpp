#ifndef RDF4CPP_DATASET_HPP
#define RDF4CPP_DATASET_HPP

#include <rdf4cpp/rdf/Graph.hpp>
#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/query/QuadPattern.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>

#include <dice/sparse-map/sparse_map.hpp>

#include <memory>
#include <utility>

namespace rdf4cpp::rdf {


struct Dataset {
    using value_type = std::pair<IRI, Graph>;
    using iterator = void;
    using const_iterator = iterator;
    using sentinel = std::default_sentinel_t;
    using reference = void;
    using pointer = void;

    struct solution_sequence {};

private:
    storage::node::DynNodeStorage node_storage_;
    dice::sparse_map::sparse_map<storage::node::identifier::NodeBackendID, Graph> graphs_;

public:
    explicit Dataset(storage::node::DynNodeStorage node_storage = storage::node::default_node_storage);

    void add(Quad const &quad);

    [[nodiscard]] bool contains(Quad const &quad) const noexcept;

    [[nodiscard]] size_t size() const;
    [[nodiscard]] size_t size(IRI const &graph_name) const;

    Graph graph();
    Graph graph(IRI const &graph_name);

    [[nodiscard]] iterator begin() const;
    [[nodiscard]] sentinel end() const;

    [[nodiscard]] solution_sequence match(query::QuadPattern const &quad_pattern) const noexcept;

    /**
     * Serialize this dataset as <a href="https://www.w3.org/TR/n-quads/">N-Quads</a>.
     *
     * @param writer writer parts
     * @return true if serialization was successful, false if a call to flush was not able to make room
     */
    bool serialize(writer::BufWriterParts writer) const noexcept;

    /**
     * Serialize this dataset as <a href="https://www.w3.org/TR/rdf12-trig/">TriG</a>.
     * This function does not call `begin` or `flush` on the given state,
     * it just serialized the contents of this Dataset using it.
     *
     * @param state serialization state
     * @param writer writer parts
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    bool serialize_trig(writer::SerializationState &state, writer::BufWriterParts writer) const noexcept;

    /**
     * Serialize this dataset as <a href="https://www.w3.org/TR/rdf12-trig/">TriG</a>.
     * This function will internally create a SerializationState and call `begin` and `flush`
     * on it when appropriate.
     *
     * @param writer writer parts
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    bool serialize_trig(writer::BufWriterParts writer) const noexcept;

    friend std::ostream &operator<<(std::ostream &os, Dataset const &self);

    // TODO: support union (+) and difference (-)
    // TODO: add empty
};


}  // namespace rdf4cpp::rdf
#endif  //RDF4CPP_DATASET_HPP
