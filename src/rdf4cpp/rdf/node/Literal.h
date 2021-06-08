//
// Created by Lixi Alié Conrads on 5/30/21.
//

#ifndef RDF4CPP_LITERAL_H
#define RDF4CPP_LITERAL_H

#include "RDFNode.h"

namespace rdf4cpp::rdf::node {
class Literal : public RDFNode {
protected:
    Literal(void *ptr, const ID &id);

public:
    [[nodiscard]] std::string as_string(bool quoting) const {
        return id_.literal().as_string(quoting);
    };

    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_LITERAL_H
