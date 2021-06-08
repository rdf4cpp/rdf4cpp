//
// Created by Lixi Alié Conrads on 5/30/21.
//

#ifndef RDF4CPP_RESOURCENODE_H
#define RDF4CPP_RESOURCENODE_H

#include "RDFNode.h"

namespace rdf4cpp::rdf::node {

class ResourceNode : public RDFNode {
public:
    [[nodiscard]] std::string as_string([[maybe_unused]] bool quoting) const { return ""; };

protected:
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_RESOURCENODE_H
