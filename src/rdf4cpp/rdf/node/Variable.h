#ifndef RDF4CPP_VARIABLE_H
#define RDF4CPP_VARIABLE_H


#include "RDFNode.h"

namespace rdf4cpp::rdf::node {
class Variable : public RDFNode {
protected:
    Variable(void *ptr, const ID &id) : RDFNode(ptr, id) {}

public:
    Variable() : RDFNode() {}

    [[nodiscard]] bool is_anonymous() const {
        return this->id_.variable().is_anonymous();
    }

    [[nodiscard]] const std::string &name() const {
        return this->id_.variable().name();
    }

    [[nodiscard]] std::string as_string(bool quoting) const {
        return id_.variable().as_string(quoting);
    };

    [[nodiscard]] bool is_blank_node() const { return false; }
    [[nodiscard]] bool is_literal() const { return false; }
    [[nodiscard]] bool is_variable() const { return true; }
    [[nodiscard]] bool is_bnode() const { return false; }
    [[nodiscard]] bool is_iri() const { return false; }
    [[nodiscard]] RDFNodeType type() const { return RDFNodeType::Variable; }

    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node

#endif  //RDF4CPP_VARIABLE_H
