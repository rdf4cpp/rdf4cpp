//
// Created by Lixi Alié Conrads on 5/30/21.
//

#ifndef RDF4CPP_BLANKNODE_H
#define RDF4CPP_BLANKNODE_H

#include "RDFNode.h"
#include "ResourceNode.h"
#include <optional>

namespace rdf4cpp::rdf::node {
class BlankNode : public RDFNode {
public:
    BlankNode();
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const { return ""; };
    [[nodiscard]] bool is_blank_node() const { return true; };
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_BLANKNODE_H
