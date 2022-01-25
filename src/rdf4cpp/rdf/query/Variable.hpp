#ifndef RDF4CPP_VARIABLE_HPP
#define RDF4CPP_VARIABLE_HPP

#include <ostream>
#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf::query {
class Variable : public Node {
protected:
    Variable(const NodeID &id);
    explicit Variable(Node::NodeBackendHandle handle);

public:
    Variable();

    explicit Variable(std::string_view name, bool anonymous = false,
                      NodeStorage &node_storage = NodeStorage::primary_instance());

    [[nodiscard]] bool is_anonymous() const;

    [[nodiscard]] std::string_view name() const;

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
