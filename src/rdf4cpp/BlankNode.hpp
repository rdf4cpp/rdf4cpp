#ifndef RDF4CPP_BLANKNODE_HPP
#define RDF4CPP_BLANKNODE_HPP

#include <ostream>
#include <rdf4cpp/Node.hpp>
#include <rdf4cpp/util/TriBool.hpp>

namespace rdf4cpp::util  {
struct NodeGenerator;
}

namespace rdf4cpp {
class BlankNode : public Node {
    explicit BlankNode(NodeBackendHandle handle) noexcept;
public:
    /**
     * Constructs the null-bnode
     */
    BlankNode() noexcept;

    /**
     * Constructs a bnode from an identifier
     */
    explicit BlankNode(std::string_view identifier, DynNodeStorage node_storage = storage::default_node_storage);

    /**
     * Constructs the null-bnode
     */
    [[nodiscard]] static BlankNode make_null() noexcept;

    /**
     * Constructs a bnode from an identifier
     */
    [[nodiscard]] static BlankNode make(std::string_view identifier, DynNodeStorage node_storage = storage::default_node_storage);

    BlankNode to_node_storage(DynNodeStorage node_storage) const noexcept;
    [[nodiscard]] BlankNode try_get_in_node_storage(DynNodeStorage node_storage) const noexcept;

    /**
     * searches for a bnode in the specified node storage and returns it.
     * returns a null bnode, if not found.
     * @param iri
     * @param node_storage
     * @return
     */
    [[nodiscard]] static BlankNode find(std::string_view identifier, DynNodeStorage node_storage = storage::default_node_storage) noexcept;

    /**
     * Get the string identifier of this. For BlankNode `_:abc` the identifier is `abc`.
     * @return string identifier
     */
    [[nodiscard]] std::string_view identifier() const noexcept;

    /**
     * See Node::serialize
     */
    bool serialize(writer::BufWriterParts writer) const noexcept;

    [[nodiscard]] explicit operator std::string() const noexcept;

    [[nodiscard]] bool merge_eq(BlankNode const &other) const noexcept;
    [[nodiscard]] util::TriBool union_eq(BlankNode const &other) const noexcept;

    friend std::ostream &operator<<(std::ostream &os, const BlankNode &node);

    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_iri() const noexcept;
    friend class Node;
};

inline namespace shorthands {

BlankNode operator""_bnode(char const *str, size_t len);

}  // namespace shorthands
}  // namespace rdf4cpp

template<>
struct std::hash<rdf4cpp::BlankNode> {
    inline size_t operator()(rdf4cpp::BlankNode const &v) const noexcept {
        return std::hash<rdf4cpp::Node>()(v);
    }
};

#endif  //RDF4CPP_BLANKNODE_HPP
