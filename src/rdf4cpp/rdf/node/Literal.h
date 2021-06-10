#ifndef RDF4CPP_LITERAL_H
#define RDF4CPP_LITERAL_H

#include <rdf4cpp/rdf/node/RDFNode.h>

namespace rdf4cpp::rdf::node {
class Literal : public RDFNode {
protected:
    Literal(void *ptr, const NodeID &id);

public:
    Literal();

    [[nodiscard]] const std::string &lexical_form() const;

    [[nodiscard]] IRIResource datatype(NodeManager &node_manager = NodeManager::default_instance()) const;

    [[nodiscard]] NodeID datatype_id() const {
        return handle_.literal_backend().datatype_id();
    }

    [[nodiscard]] const std::string &language_tag() const;


    [[nodiscard]] std::string as_string(bool quoting = false,
                                        NodeManager &node_manager = NodeManager::default_instance()) const;

    [[nodiscard]] bool is_blank_node() const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    friend class RDFNode;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_LITERAL_H
