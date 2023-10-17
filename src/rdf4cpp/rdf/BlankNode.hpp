#ifndef RDF4CPP_BLANKNODE_HPP
#define RDF4CPP_BLANKNODE_HPP

#include <optional>

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf {
class BlankNode : public Node {

private:
    explicit BlankNode(NodeBackendHandle handle) noexcept;

public:
    /**
     * Constructs the null-bnode
     */
    BlankNode() noexcept;

    /**
     * Constructs a bnode from an identifier
     */
    explicit BlankNode(std::string_view identifier, NodeStorage &node_storage = NodeStorage::default_instance());

    /**
     * Constructs the null-bnode
     */
    [[nodiscard]] static BlankNode make_null() noexcept;

    /**
     * Constructs a bnode from an identifier
     */
    [[nodiscard]] static BlankNode make(std::string_view identifier, NodeStorage &node_storage = NodeStorage::default_instance());

    BlankNode to_node_storage(NodeStorage &node_storage) const noexcept;
    [[nodiscard]] BlankNode try_get_in_node_storage(NodeStorage const &node_storage) const noexcept;

    /**
     * Get the string identifier of this. For BlankNode `_:abc` the identifier is `abc`.
     * @return string identifier
     */
    [[nodiscard]] std::string_view identifier() const noexcept;

    [[nodiscard]] explicit operator std::string() const noexcept;

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
}  // namespace rdf4cpp::rdf

template<>
struct std::hash<rdf4cpp::rdf::BlankNode> {
    inline size_t operator()(rdf4cpp::rdf::BlankNode const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::BlankNode> {
    static size_t dice_hash(rdf4cpp::rdf::BlankNode const &v) noexcept {
        return DiceHash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_BLANKNODE_HPP
