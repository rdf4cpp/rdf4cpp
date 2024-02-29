#ifndef RDF4CPP_QUAD_HPP
#define RDF4CPP_QUAD_HPP

#include <rdf4cpp/rdf/query/QuadPattern.hpp>

namespace rdf4cpp::rdf {
namespace writer {
struct SerializationState;
}
/**
 * A Quad is an RDF statement which has an additional graph name.
 * Quads are typically used to identify a statement in a named Graph of a RDF Dataset. The Graph name can also be the default Graph.
 */
class Quad : public query::QuadPattern {
    // TODO: more type-specific functions for graph, subject, predicate, object
public:
    /**
     * Initializes the Quad entries with null <div>Node</div>s
     */
    Quad() = default;

    /**
     * The constructed Quad has the default Graph as graph name. The Quad is not validated.
     * @param subject IRI or BlankNode
     * @param predicate IRI
     * @param object IRI, Literal or BlankNode
     */
    Quad(Node subject, Node predicate, Node object);

    /**
     * Construct a Quad. The Quad is not validated.
     * @param graph IRI or BlankNode
     * @param subject IRI or BlankNode
     * @param predicate IRI
     * @param object IRI, Literal or BlankNode
     */
    Quad(Node graph, Node subject, Node predicate, Node object);


    [[nodiscard]] bool valid() const noexcept;

    /**
     * The constructed Quad is validated. The optional result is only present, if it is valid.
     * @param graph IRO or BlankNode, name of the graph where the statement (subject, predicate, object) is added
     * @param subject IRI or BlankNode
     * @param predicate IRI
     * @param object IRI, Literal or BlankNode
     * @return A Quad if valid otherwise std::optional is empty
     */
    static std::optional<Quad> create_validated(Node graph, Node subject, Node predicate, Node object) noexcept;

    /**
     * The constructed Quad has the default Graph as graph name and is validated. The optional result is only present, if it is valid.
     * @param subject IRI or BlankNode
     * @param predicate IRI
     * @param object IRI, Literal or BlankNode
     * @return A Quad if valid otherwise std::optional is empty
     */
    static std::optional<Quad> create_validated(Node subject, Node predicate, Node object) noexcept;

    [[nodiscard]] Quad to_node_storage(storage::node::NodeStorage &node_storage) const noexcept;

    bool serialize_ntriples(writer::BufWriterParts parts) const noexcept;
    bool serialize_nquads(writer::BufWriterParts parts) const noexcept;
    bool serialize_turtle(writer::SerializationState &state, writer::BufWriterParts parts) const noexcept;
    bool serialize_trig(writer::SerializationState &state, writer::BufWriterParts parts) const noexcept;
};
}  // namespace rdf4cpp::rdf

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::Quad> {
    static inline size_t dice_hash(rdf4cpp::rdf::Quad const &x) noexcept {
        return dice::hash::dice_hash_templates<Policy>::dice_hash(std::tie(x.graph(), x.subject(), x.predicate(), x.object()));
    }
};

#endif  //RDF4CPP_QUAD_HPP
