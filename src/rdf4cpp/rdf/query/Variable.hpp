#ifndef RDF4CPP_VARIABLE_HPP
#define RDF4CPP_VARIABLE_HPP

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf::query {
class Variable : public Node {
protected:
    explicit Variable(Node::NodeBackendHandle handle) noexcept;

public:
    Variable() noexcept;

    explicit Variable(std::string_view name, bool anonymous = false,
                      NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] static Variable make_named(std::string_view name, NodeStorage &node_storage = NodeStorage::default_instance());
    [[nodiscard]] static Variable make_anonymous(std::string_view name, NodeStorage &node_storage = NodeStorage::default_instance());

    Variable to_node_storage(NodeStorage &node_storage) const noexcept;
    [[nodiscard]] Variable try_get_in_node_storage(NodeStorage const &node_storage) const noexcept;

    [[nodiscard]] bool is_anonymous() const;

    [[nodiscard]] std::string_view name() const;

    bool serialize(void *stream, Sink sink) const noexcept;
    [[nodiscard]] explicit operator std::string() const;
    friend std::ostream &operator<<(std::ostream &os, const Variable &variable);

    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    friend class Node;

    // todo unbound()
};

}  // namespace rdf4cpp::rdf::query

template<>
struct std::hash<rdf4cpp::rdf::query::Variable> {
    inline size_t operator()(rdf4cpp::rdf::query::Variable const &v) const noexcept {
        return std::hash<rdf4cpp::rdf::Node>()(v);
    }
};

#endif  //RDF4CPP_VARIABLE_HPP
