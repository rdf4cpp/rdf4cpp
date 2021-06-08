#ifndef RDF4CPP_RESOURCEMANAGER_H
#define RDF4CPP_RESOURCEMANAGER_H

#include "BNodeBackend.h"
#include "ID.h"
#include "IRIBackend.h"
#include "LiteralBackend.h"
#include "VariableBackend.h"
#include <map>
#include <memory>
namespace rdf4cpp::rdf::graph::node_manager {

class ResourceManager {
    std::map<ID, std::unique_ptr<LiteralBackend>, std::less<>> literal_storage;
    std::map<LiteralBackend *, ID, std::less<>> literal_storage_reverse;
    std::map<ID, std::unique_ptr<BNodeBackend>, std::less<>> bnode_storage;
    std::map<BNodeBackend *, ID, std::less<>> bnode_storage_reverse;
    std::map<ID, std::unique_ptr<IRIBackend>, std::less<>> iri_storage;
    std::map<IRIBackend *, ID, std::less<>> iri_storage_reverse;
    std::map<ID, std::unique_ptr<VariableBackend>, std::less<>> variable_storage;
    std::map<VariableBackend *, ID, std::less<>> variable_storage_reverse;

    size_t next_literal_id = ID(0, RDFNodeType::Literal).id();
    size_t next_bnode_id = ID(0, RDFNodeType::BNode).id();
    size_t next_iri_id = ID(2, RDFNodeType::IRI).id();
    size_t next_variable_id = ID(0, RDFNodeType::Variable).id();

    const ID xsd_string_iri_id = ID(0, RDFNodeType::IRI);
    const ID xsd_langstring_iri_id = ID(1, RDFNodeType::IRI);

public:
    ResourceManager();

private:
    std::pair<LiteralBackend *, ID> lookup_or_insert_literal(LiteralBackend literal);

    std::pair<IRIBackend *, ID> lookup_or_insert_iri(IRIBackend iri);

    std::pair<BNodeBackend *, ID> lookup_or_insert_bnode(BNodeBackend bnode);

    std::pair<VariableBackend *, ID> lookup_or_insert_variable(VariableBackend variable);

public:
    std::pair<LiteralBackend *, ID> getLiteral(const std::string &lexical_form);

    std::pair<LiteralBackend *, ID> getTypedLiteral(const std::string &lexical_form, const std::string &datatype);

    std::pair<LiteralBackend *, ID> getLangLiteral(const std::string &lexical_form, const std::string &lang);

    std::pair<IRIBackend *, ID> getIRI(const std::string &iri);

    /**
     *
     * @param name name without _:
     * @return
     */
    std::pair<BNodeBackend *, ID> getBnode(const std::string &identifier);

    ID lookup_id(void const *const ptr, const RDFNodeType node_type);

    /**
     *
     * @param name name without _:
     * @return
     */
    std::pair<VariableBackend *, ID> getVariable(const std::string &identifier, bool anonymous = false);

    static ResourceManager &default_instance();
};

}  // namespace rdf4cpp::rdf::graph::node_manager
#endif  //RDF4CPP_RESOURCEMANAGER_H
