#ifndef RDF4CPP_VARIABLE_H
#define RDF4CPP_VARIABLE_H

#include <rdf4cpp/rdf/node/RDFNode.h>

namespace rdf4cpp::rdf::node {
class Variable : public RDFNode {
protected:
    Variable(void *ptr, const NodeID &id);

public:
    Variable();

    [[nodiscard]] bool is_anonymous() const;

    [[nodiscard]] const std::string &name() const;

    [[nodiscard]] std::string as_string(bool quoting = false) const;

    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node

#endif  //RDF4CPP_VARIABLE_H
