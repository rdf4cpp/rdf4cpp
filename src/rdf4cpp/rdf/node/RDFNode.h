#ifndef RDF4CPP_RDFNODE_H
#define RDF4CPP_RDFNODE_H

#include <optional>
#include <string>

#include <rdf4cpp/rdf/graph/node_manager/BackendNodeHandle.h>
#include <rdf4cpp/rdf/graph/node_manager/NodeManager.h>

namespace rdf4cpp::rdf::node {
class Literal;
class BlankNode;
class IRIResource;
class Variable;

/**
 * The abstract RDFNode class, containing either a Literal, Variable, IRIResource or BlankNode
 */
class RDFNode {
protected:
    using BackendNodeHandle = rdf4cpp::rdf::graph::node_manager::BackendNodeHandle;
    using NodeID = rdf4cpp::rdf::graph::node_manager::NodeID;
    using NodeManager = rdf4cpp::rdf::graph::node_manager::NodeManager;
    using RDFNodeType = rdf4cpp::rdf::graph::node_manager::RDFNodeType;
    BackendNodeHandle handle_;

    RDFNode(void *ptr, NodeID id);
    explicit RDFNode(const BackendNodeHandle &id);

public:
    RDFNode() = default;

    static Literal make_string_literal(const std::string &lexical_form,
                                       NodeManager &node_manager = NodeManager::default_instance());


    static Literal make_typed_literal(const std::string &lexical_form, const std::string &datatype,
                                      NodeManager &node_manager = NodeManager::default_instance());

    static Literal make_typed_literal(const std::string &lexical_form, const IRIResource &datatype,
                                      NodeManager &node_manager = NodeManager::default_instance());

    static Literal make_lang_literal(const std::string &lexical_form, const std::string &lang,
                                     NodeManager &node_manager = NodeManager::default_instance());

    static IRIResource make_iri(const std::string &iri,
                                NodeManager &node_manager = NodeManager::default_instance());

    /**
     *
     * @param identifier name name without _: or ?
     * @param anonymous true if string repr. it starts with ? and false if it starts with ?
     * @return
     */
    static Variable make_variable(const std::string &identifier, bool anonymous = false,
                                  NodeManager &node_manager = NodeManager::default_instance());

    /**
     *
     * @param name name without _:
     * @return
     */
    static BlankNode make_bnode(const std::string &identifier,
                                NodeManager &node_manager = NodeManager::default_instance());


    [[nodiscard]] std::string as_string(bool quoting = false,
                                        NodeManager &node_manager = NodeManager::default_instance()) const;
    [[nodiscard]] bool is_literal() const;
    [[nodiscard]] bool is_variable() const;
    [[nodiscard]] bool is_bnode() const;
    [[nodiscard]] bool is_iri() const;
    [[nodiscard]] RDFNodeType type() const;

    bool operator==(const RDFNode &other) const;

    std::strong_ordering operator<=>(const RDFNode &other) const;
};
}  // namespace rdf4cpp::rdf::node


#endif  //RDF4CPP_RDFNODE_H
