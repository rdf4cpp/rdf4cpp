#ifndef RDF4CPP_GRAPH_HPP
#define RDF4CPP_GRAPH_HPP

#include <rdf4cpp/rdf/Statement.hpp>
#include <rdf4cpp/rdf/query/TriplePattern.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>

#include <dice/sparse-map/sparse_set.hpp>

namespace rdf4cpp::rdf {

struct Graph {
    using value_type = Statement;
    using allocator_type = std::allocator<Statement>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = Statement const &;
    using const_reference = reference;
    using pointer = Statement const *;
    using const_pointer = pointer;

private:
    struct triple {
        storage::node::identifier::NodeBackendID subject;
        storage::node::identifier::NodeBackendID predicate;
        storage::node::identifier::NodeBackendID object;

        bool operator==(triple const &) const noexcept = default;
    };

    struct triple_hash {
        size_t operator()(triple const &trip) const noexcept {
            return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(std::make_tuple(trip.subject, trip.predicate, trip.object));
        }
    };

    using triple_storage_type = dice::sparse_map::sparse_set<triple, triple_hash>;

public:
    using sentinel = std::default_sentinel_t;

    struct iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = Statement;
        using difference_type = ptrdiff_t;
        using pointer = Statement const *;
        using reference = Statement const &;

    private:
        typename triple_storage_type::const_iterator iter_;
        typename triple_storage_type::const_iterator end_;
        Graph const *parent_;

        Statement cur_;

    public:
        iterator(typename triple_storage_type::const_iterator beg, typename triple_storage_type::const_iterator end, Graph const *parent) noexcept;

        iterator &operator++() noexcept;
        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        bool operator==(sentinel) const noexcept;
        bool operator!=(sentinel) const noexcept;
    };

    using const_iterator = iterator;

    struct solution_iterator {
        using iterator_category = std::input_iterator_tag;
        using value_type = Statement;
        using difference_type = ptrdiff_t;
        using pointer = Statement const *;
        using reference = Statement const &;

    private:
        typename triple_storage_type::const_iterator iter_;
        typename triple_storage_type::const_iterator end_;
        Graph const *parent_;

        query::TriplePattern pat_;
        Statement cur_;

        bool check_solution() noexcept;
        void forward_to_solution() noexcept;

    public:
        solution_iterator(typename triple_storage_type::const_iterator beg,
                          typename triple_storage_type::const_iterator end,
                          Graph const *parent,
                          query::TriplePattern const &pat) noexcept;

        solution_iterator &operator++() noexcept;
        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        bool operator==(sentinel) const noexcept;
        bool operator!=(sentinel) const noexcept;
    };

    struct solution_sequence {
        using value_type = Statement;
        using allocator_type = std::allocator<Statement>;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using reference = Statement const &;
        using const_reference = reference;
        using pointer = Statement const *;
        using const_pointer = pointer;
        using iterator = solution_iterator;
        using const_iterator = solution_iterator;

    private:
        iterator beg_;

    public:
        explicit solution_sequence(iterator beg) noexcept : beg_{beg} {
        }

        [[nodiscard]] iterator begin() const noexcept {
            return beg_;
        }

        [[nodiscard]] sentinel end() const noexcept {
            return sentinel{};
        }
    };

private:
    storage::node::DynNodeStorage node_storage_;
    triple_storage_type triples_;

    static storage::node::identifier::NodeBackendID to_node_id(Node node) noexcept;
    Node to_node(storage::node::identifier::NodeBackendID id) const noexcept;

public:
    explicit Graph(storage::node::DynNodeStorage node_storage = storage::node::default_node_storage) noexcept;

    void add(Statement const &statement);

    [[nodiscard]] size_t size() const noexcept;
    [[nodiscard]] bool contains(Statement const &statement) const noexcept;

    [[nodiscard]] solution_sequence match(query::TriplePattern const &triple_pattern) const noexcept;

    [[nodiscard]] iterator begin() const noexcept;
    [[nodiscard]] sentinel end() const noexcept;

    /**
     * Serialize this graph as <a href="https://www.w3.org/TR/n-triples/">N-Triples</a>.
     *
     * @param writer writer parts
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    bool serialize(writer::BufWriterParts writer) const noexcept;

    /**
     * Serialize this graph as <a href="https://www.w3.org/TR/rdf12-turtle/">Turtle</a>
     * This function does not call `begin` or `flush` on the given state,
     * it just serialized the contents of this Graph using it.
     *
     * @param writer writer parts
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    bool serialize_turtle(writer::SerializationState &state, writer::BufWriterParts writer) const noexcept;

    /**
     * Serialize this graph as <a href="https://www.w3.org/TR/rdf12-turtle/">Turtle</a>
     * This function will internally create a SerializationState and call `begin` and `flush`
     * on it when appropriate.
     *
     * @param writer writer parts
     * @return true if serialization was successful, false if a call to W::flush was not able to make room
     */
    bool serialize_turtle(writer::BufWriterParts writer) const noexcept;

    /**
     * Serialize this graph as <a href="https://www.w3.org/TR/n-triples/">N-Triples</a>.
     */
    friend std::ostream &operator<<(std::ostream &os, Graph const &graph);

    // TODO: support union (+) and difference (-); open question: which graph name should be assigned?
    // TODO: add empty
};
}  // namespace rdf4cpp::rdf

#endif  //RDF4CPP_GRAPH_HPP
