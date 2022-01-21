#ifndef RDF4CPP_BLANKNODE_HPP
#define RDF4CPP_BLANKNODE_HPP

#include <optional>

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf {
class BlankNode : public Node {

private:
    BlankNode(const NodeID &id);
    explicit BlankNode(BackendNodeHandle handle);

public:
    BlankNode();
    explicit BlankNode(const std::string &identifier,
                       NodeStorage &node_storage = NodeStorage::primary_instance());

    /**
     * Get the string identifier of this. For BlankNode `_:abc` the identifier is `abc`.
     * @return string identifier
     */
    [[nodiscard]] std::string_view identifier() const;

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const BlankNode &node);
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_iri() const;
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
