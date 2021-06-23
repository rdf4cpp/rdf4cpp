#ifndef RDF4CPP_LITERAL_H
#define RDF4CPP_LITERAL_H

#include <rdf4cpp/rdf/node/RDFNode.h>

namespace rdf4cpp::rdf::node {
class Literal : public RDFNode {
protected:
    Literal(const NodeID &id);
    explicit Literal(RDFNode::BackendNodeHandle handle);

public:
    Literal();
    explicit Literal(const std::string &lexical_form,
                     NodeManager &node_storage = NodeManager::primary_instance());
    Literal(const std::string &lexical_form, const IRIResource &datatype,
            NodeManager &node_storage = NodeManager::primary_instance());
    Literal(const std::string &lexical_form, const std::string &lang,
            NodeManager &node_storage = NodeManager::primary_instance());

    [[nodiscard]] const std::string &lexical_form() const;

    [[nodiscard]] IRIResource datatype() const;

    [[nodiscard]] const std::string &language_tag() const;


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


#endif  //RDF4CPP_LITERAL_H
