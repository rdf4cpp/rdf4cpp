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
    Literal() : RDFNode() {}

    [[nodiscard]] const std::string &lexical_form() const {
        return id_.literal().lexical_form();
    }

    [[nodiscard]] IRIResource datatype(ResourceManager &node_manager = ResourceManager::default_instance()) const;

    [[nodiscard]] ID datatype_id() const {
        return id_.literal().datatype_id();
    }

    [[nodiscard]] const std::string &language_tag() const {
        return id_.literal().language_tag();
    }


    [[nodiscard]] std::string as_string(bool quoting) const {
        return id_.literal().as_string(quoting);
    };

    [[nodiscard]] bool is_blank_node() const { return false; }
    [[nodiscard]] bool is_literal() const { return true; }
    [[nodiscard]] bool is_variable() const { return false; }
    [[nodiscard]] bool is_bnode() const { return false; }
    [[nodiscard]] bool is_iri() const { return false; }
    [[nodiscard]] RDFNodeType type() const { return RDFNodeType::Literal; }

    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_LITERAL_H
