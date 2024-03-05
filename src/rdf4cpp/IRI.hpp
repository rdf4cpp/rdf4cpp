#ifndef RDF4CPP_IRI_HPP
#define RDF4CPP_IRI_HPP

#include <ostream>
#include <rdf4cpp/Node.hpp>
#include <rdf4cpp/datatypes/registry/DatatypeID.hpp>

namespace rdf4cpp {

/**
 * IRI Resource node.
 */
class IRI : public Node {
private:
    /**
     * Constructs the corresponding IRI from a given datatype id and places it into node_storage if
     * it does not exist already.
     */
    IRI(datatypes::registry::DatatypeIDView id, DynNodeStorage node_storage) noexcept;
    
    /**
     * Constructs the corresponding datatype id for this iri. Return value can be safely used to
     * index the registry and yields the correct result.
     */
    explicit operator datatypes::registry::DatatypeIDView() const noexcept;

public:
    explicit IRI(Node::NodeBackendHandle handle) noexcept;

    /**
     * Constructs the null-iri
     */
    IRI() noexcept;

    /**
     * Constructs an IRI object from a IRI string
     * @param iri IRI string
     * @param node_storage optional custom node_storage used to store the IRI
     */
    explicit IRI(std::string_view iri, DynNodeStorage node_storage = storage::default_node_storage);

    /**
     * Constructs the null-iri
     */
    [[nodiscard]] static IRI make_null() noexcept;

    /**
     * Constructs an IRI object from a IRI string
     * @param iri IRI string
     * @param node_storage optional custom node_storage used to store the IRI
     */
    [[nodiscard]] static IRI make(std::string_view iri, DynNodeStorage node_storage = storage::default_node_storage);

    /**
     * creates a new IRI containing a random UUID (Universally Unique IDentifier)
     * @return UUID IRI
     */
    [[nodiscard]] static IRI make_uuid(DynNodeStorage node_storage = storage::default_node_storage);

    IRI to_node_storage(DynNodeStorage node_storage) const noexcept;
    [[nodiscard]] IRI try_get_in_node_storage(DynNodeStorage node_storage) const noexcept;

    /**
     * searches for a IRI in the specified node storage and returns it.
     * returns a null IRI, if not found.
     * @param iri
     * @param node_storage
     * @return
     */
    [[nodiscard]] static IRI find(std::string_view iri, DynNodeStorage node_storage = storage::default_node_storage) noexcept;
private:
    /**
     * searches for a IRI in the specified node storage and returns it.
     * returns a null IRI, if not found.
     * @param iri
     * @param node_storage
     * @return
     */
    [[nodiscard]] static IRI find(datatypes::registry::DatatypeIDView id, DynNodeStorage node_storage) noexcept;

public:
    /**
     * Get the IRI string of this.
     * @return IRI string
     */
    [[nodiscard]] std::string_view identifier() const noexcept;

    /**
     * See Node::serialize
     */
    bool serialize(writer::BufWriterParts writer) const noexcept;

    [[nodiscard]] explicit operator std::string() const noexcept;
    friend std::ostream &operator<<(std::ostream &os, const IRI &iri);

    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_iri() const noexcept;

    friend class Node;
    friend class Literal;

    /**
     * Get the default graph IRI.
     * @param node_storage  optional custom node_storage where the returned IRI lives
     * @return default graph IRI
     */
    static IRI default_graph(DynNodeStorage node_storage = storage::default_node_storage);
};

inline namespace shorthands {

IRI operator""_iri(char const *str, size_t len);

}  // namespace shorthands
}  // namespace rdf4cpp

template<>
struct std::hash<rdf4cpp::IRI> {
    inline size_t operator()(rdf4cpp::IRI const &v) const noexcept {
        return std::hash<rdf4cpp::Node>()(v);
    }
};

#endif  //RDF4CPP_IRI_HPP
