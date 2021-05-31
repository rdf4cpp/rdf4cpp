//
// Created by Lixi Alié Conrads on 5/30/21.
//

#ifndef RDF4CPP_BLANKNODE_H
#define RDF4CPP_BLANKNODE_H

#include "RDFNode.h"
#include "ResourceNode.h"
#include <optional>

namespace rdf4cpp::rdf::node {
    class BlankNode : public ResourceNode {
    public:
        BlankNode()= default;
        [[nodiscard]] std::string as_string(bool quoting)const override{ return "";};
        [[nodiscard]] bool is_blank_node() const override{ return true; };
    };
}


#endif //RDF4CPP_BLANKNODE_H
