//
// Created by Lixi Alié Conrads on 5/30/21.
//

#ifndef RDF4CPP_BLANKNODE_H
#define RDF4CPP_BLANKNODE_H

#include "RDFNode.h"
#include <optional>

namespace rdf4cpp::rdf::node {
class BlankNode : public RDFNode {

public:
    BlankNode(void *ptr, const ID &id);

public:
    BlankNode();
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const;
    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const { return false; }
    [[nodiscard]] bool is_variable() const { return false; }
    [[nodiscard]] bool is_bnode() const { return false; }
    [[nodiscard]] bool is_iri() const { return false; }
    [[nodiscard]] RDFNodeType type() const { return RDFNodeType::BNode; }
    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_BLANKNODE_H
