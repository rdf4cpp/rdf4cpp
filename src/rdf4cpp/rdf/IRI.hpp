#ifndef RDF4CPP_IRI_HPP
#define RDF4CPP_IRI_HPP

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf {

/**
 * IRI Resource node.
 */
class IRI : public Node {
public:
    IRI(const NodeID &id);
    explicit IRI(Node::BackendNodeHandle handle);


public:
    IRI();

    /**
     * Constructs an IRI object from a IRI string
     * @param iri IRI string
     * @param node_storage optional custom node_storage used to store the IRI
     */
    explicit IRI(std::string_view iri,
                 NodeStorage &node_storage = NodeStorage::primary_instance());

    /**
     * Get the IRI string of this.
     * @return IRI string
     */
    [[nodiscard]] std::string_view identifier() const;

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const IRI &iri);

    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_iri() const;

    friend class Node;
    friend class Literal;

    /**
     * Get the default graph IRI.
     * @param node_storage  optional custom node_storage where the returned IRI lives
     * @return default graph IRI
     */
    static IRI default_graph(NodeStorage &node_storage = NodeStorage::primary_instance());
};
}  // namespace rdf4cpp::rdf

template<>
struct std::hash<rdf4cpp::rdf::IRI> {
    inline size_t operator()(rdf4cpp::rdf::IRI const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_IRI_HPP
