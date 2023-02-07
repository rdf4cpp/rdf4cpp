#ifndef RDF4CPP_BLANKNODE_HPP
#define RDF4CPP_BLANKNODE_HPP

#include <optional>

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf::util  {
struct NodeGenerator;
}

namespace rdf4cpp::rdf {
class BlankNode : public Node {
public:
    explicit BlankNode(NodeBackendHandle handle) noexcept;

    BlankNode() noexcept;

    explicit BlankNode(std::string_view identifier,
                       NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] BlankNode to_node_storage(NodeStorage &node_storage = NodeStorage::default_instance()) const noexcept;

    /**
     * Get the string identifier of this. For BlankNode `_:abc` the identifier is `abc`.
     * @return string identifier
     */
    [[nodiscard]] std::string_view identifier() const noexcept;

    [[nodiscard]] explicit operator std::string() const noexcept;

    [[nodiscard]] bool merge_eq(BlankNode const &other) const noexcept;
    [[nodiscard]] std::optional<bool> union_eq(BlankNode const &other) const noexcept;

    friend std::ostream &operator<<(std::ostream &os, const BlankNode &node);
    [[nodiscard]] bool is_literal() const noexcept;
    [[nodiscard]] bool is_variable() const noexcept;
    [[nodiscard]] bool is_blank_node() const noexcept;
    [[nodiscard]] bool is_iri() const noexcept;
    friend class Node;
};
}  // namespace rdf4cpp::rdf

template<>
struct std::hash<rdf4cpp::rdf::BlankNode> {
    inline size_t operator()(rdf4cpp::rdf::BlankNode const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_BLANKNODE_HPP
