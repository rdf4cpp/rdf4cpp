//
// Created by Lixi Alié Conrads on 5/30/21.
//

#ifndef RDF4CPP_IRIRESOURCE_H
#define RDF4CPP_IRIRESOURCE_H

#include "RDFNode.h"

namespace rdf4cpp::rdf::node {

///
/// IRI resource node.
///
class IRIResource : public RDFNode {
public:
    IRIResource(void *ptr, const ID &id);

public:
    IRIResource() : RDFNode(){};
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const { return ""; };

    [[nodiscard]] bool is_blank_node() const { return false; }
    [[nodiscard]] bool is_literal() const { return false; }
    [[nodiscard]] bool is_variable() const { return false; }
    [[nodiscard]] bool is_bnode() const { return false; }
    [[nodiscard]] bool is_iri() const { return true; }
    [[nodiscard]] RDFNodeType type() const { return RDFNodeType::IRI; }
    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_IRIRESOURCE_H
