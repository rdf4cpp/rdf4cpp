#ifndef RDF4CPP_IRI_HPP
#define RDF4CPP_IRI_HPP

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/datatypes/registry/DatatypeID.hpp>

namespace rdf4cpp::rdf {

/**
 * IRI Resource node.
 */
class IRI : public Node {
private:
    /**
     * Constructs the corresponding IRI from a given datatype id and places it into node_storage if
     * it does not exist already.
     */
    static IRI from_datatype_id(datatypes::registry::DatatypeIDView id, NodeStorage &node_storage = NodeStorage::default_instance()) noexcept;

    /**
     * Constructs the corresponding datatype id for this iri. Return value can be safely used to
     * index the registry and yields the correct result.
     */
    [[nodiscard]] datatypes::registry::DatatypeIDView to_datatype_id() const noexcept;

public:
    explicit IRI(Node::NodeBackendHandle handle) noexcept;

    IRI() noexcept;

    /**
     * Constructs an IRI object from a IRI string
     * @param iri IRI string
     * @param node_storage optional custom node_storage used to store the IRI
     */
    explicit IRI(std::string_view iri,
                 NodeStorage &node_storage = NodeStorage::default_instance());

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
    static IRI default_graph(NodeStorage &node_storage = NodeStorage::default_instance());
};
}  // namespace rdf4cpp::rdf

template<>
struct std::hash<rdf4cpp::rdf::IRI> {
    inline size_t operator()(rdf4cpp::rdf::IRI const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_IRI_HPP
