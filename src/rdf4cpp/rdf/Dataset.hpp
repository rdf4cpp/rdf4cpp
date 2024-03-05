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
    using sentinel = std::default_sentinel_t;
    using reference = void; // TODO
    using pointer = void;

private:
    using storage_type = dice::sparse_map::sparse_map<storage::identifier::NodeBackendID, Graph>;

public:
    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = Quad;
        using difference_type = ptrdiff_t;
        using pointer = value_type const *;
        using reference = value_type const &;

    private:
        Dataset const *parent_;
        typename storage_type::const_iterator giter_;
        typename storage_type::const_iterator gend_;
        typename Graph::iterator iter_;

        Quad cur_;

    public:
        iterator(Dataset const *parent,
                 typename storage_type::const_iterator gbeg,
                 typename storage_type::const_iterator gend) noexcept;

        iterator &operator++() noexcept;
        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        bool operator==(sentinel) const noexcept;
        bool operator!=(sentinel) const noexcept;
    };

    using const_iterator = iterator;

    struct solution_iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = query::Solution;
        using difference_type = ptrdiff_t;
        using pointer = value_type const *;
        using reference = value_type const &;

    private:
        Dataset const *parent_;
        query::QuadPattern pat_;

        typename storage_type::const_iterator giter_;
        typename storage_type::const_iterator gend_;

        Graph::solution_iterator iter_;
        value_type cur_;

        void fill_solution() noexcept;

    public:
        solution_iterator(Dataset const *parent,
                          query::QuadPattern const &pat,
                          typename storage_type::const_iterator beg,
                          typename storage_type::const_iterator end) noexcept;

        solution_iterator &operator++() noexcept;
        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        bool operator==(sentinel) const noexcept;
        bool operator!=(sentinel) const noexcept;
    };

    struct solution_sequence {
        using value_type = query::Solution;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = value_type const &;
        using const_reference = reference;
        using pointer = value_type const *;
        using const_pointer = pointer;
        using iterator = solution_iterator;
        using const_iterator = solution_iterator;
        using sentinel = sentinel;

    private:
        iterator beg_;

    public:
        explicit solution_sequence(iterator beg) noexcept : beg_{beg} {
        }

        [[nodiscard]] iterator begin() const noexcept {
            return beg_;
        }

        [[nodiscard]] static sentinel end() noexcept {
            return sentinel{};
        }
    };

private:
    storage::DynNodeStorage node_storage_;
    storage_type graphs_;

    static storage::identifier::NodeBackendID to_node_id(Node node) noexcept;
    Node to_node(storage::identifier::NodeBackendID id) const noexcept;

public:
    explicit Dataset(storage::DynNodeStorage node_storage = storage::default_node_storage);

    void add(Quad const &quad);

    [[nodiscard]] bool contains(Quad const &quad) const noexcept;

    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] size_t size(IRI const &graph_name) const noexcept;

    Graph *graph(Node const &graph);
    Graph *graph();

    Graph const *graph(Node const &graph) const;
    Graph const *graph() const;


    [[nodiscard]] iterator begin() const noexcept;
    [[nodiscard]] sentinel end() const noexcept;

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
