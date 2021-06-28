#ifndef RDF4CPP_IRI_HPP
#define RDF4CPP_IRI_HPP

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

    explicit IRI(const std::string &iri,
                 NodeStorage &node_storage = NodeStorage::primary_instance());

    /**
     * Returns a string representation. Quoting has no effect on IRI.
     * @param quoting
     * @return
     */
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting = false) const;

    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;
    friend class Node;
    friend class Literal;

    static IRI default_graph(NodeStorage &node_storage = NodeStorage::primary_instance());
};
}  // namespace rdf4cpp::rdf


#endif  //RDF4CPP_IRI_HPP
