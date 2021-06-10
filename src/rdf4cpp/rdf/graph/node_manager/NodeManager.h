#ifndef RDF4CPP_NODEMANAGER_H
#define RDF4CPP_NODEMANAGER_H

#include <rdf4cpp/rdf/graph/node_manager/BNodeBackend.h>
#include <rdf4cpp/rdf/graph/node_manager/IRIBackend.h>
#include <rdf4cpp/rdf/graph/node_manager/LiteralBackend.h>
#include <rdf4cpp/rdf/graph/node_manager/NodeID.h>
#include <rdf4cpp/rdf/graph/node_manager/VariableBackend.h>

#include <map>
#include <memory>
namespace rdf4cpp::rdf::graph::node_manager {

class NodeManager {
    std::map<NodeID, std::unique_ptr<LiteralBackend>, std::less<>> literal_storage;
    std::map<LiteralBackend *, NodeID, std::less<>> literal_storage_reverse;
    std::map<NodeID, std::unique_ptr<BNodeBackend>, std::less<>> bnode_storage;
    std::map<BNodeBackend *, NodeID, std::less<>> bnode_storage_reverse;
    std::map<NodeID, std::unique_ptr<IRIBackend>, std::less<>> iri_storage;
    std::map<IRIBackend *, NodeID, std::less<>> iri_storage_reverse;
    std::map<NodeID, std::unique_ptr<VariableBackend>, std::less<>> variable_storage;
    std::map<VariableBackend *, NodeID, std::less<>> variable_storage_reverse;

    size_t next_literal_id = NodeID::min_literal_id;
    size_t next_bnode_id = NodeID::min_bnode_id;
    size_t next_iri_id = NodeID::min_iri_id;
    size_t next_variable_id = NodeID::min_variable_id;


public:
    NodeManager();

private:
    std::pair<LiteralBackend *, NodeID> lookup_or_insert_literal(LiteralBackend literal);

    std::pair<IRIBackend *, NodeID> lookup_or_insert_iri(IRIBackend iri);

    std::pair<BNodeBackend *, NodeID> lookup_or_insert_bnode(BNodeBackend bnode);

    std::pair<VariableBackend *, NodeID> lookup_or_insert_variable(VariableBackend variable);

public:
    std::pair<LiteralBackend *, NodeID> get_string_literal(const std::string &lexical_form);

    std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const std::string &datatype);
    std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const NodeID &datatype_id);

    std::pair<LiteralBackend *, NodeID> get_lang_literal(const std::string &lexical_form, const std::string &lang);

    std::pair<IRIBackend *, NodeID> get_iri(const std::string &iri);

    /**
     *
     * @param identifier name name without _: or ?
     * @param anonymous true if string repr. it starts with ? and false if it starts with ?
     * @return
     */
    std::pair<VariableBackend *, NodeID> get_variable(const std::string &identifier, bool anonymous = false);

    /**
     *
     * @param name name without _:
     * @return
     */
    std::pair<BNodeBackend *, NodeID> get_bnode(const std::string &identifier);

    NodeID lookup_id(void const *const ptr, const RDFNodeType node_type);

    IRIBackend *lookup_iri(NodeID id) const;

    LiteralBackend *lookup_literal(NodeID id) const;

    BNodeBackend *lookup_bnode(NodeID id) const;

    VariableBackend *lookup_variable(NodeID id) const;


    static NodeManager &default_instance();
};

}  // namespace rdf4cpp::rdf::graph::node_manager
#endif  //RDF4CPP_NODEMANAGER_H
