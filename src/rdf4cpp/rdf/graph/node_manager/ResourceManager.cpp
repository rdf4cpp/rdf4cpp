#include "rdf4cpp/rdf/graph/node_manager/ResourceManager.h"
namespace rdf4cpp::rdf::graph::node_manager {
ResourceManager::ResourceManager() {
    // xsd:string is there by default
    iri_storage.insert({xsd_string_iri_id, std::make_unique<IRIBackend>("http://www.w3.org/2001/XMLSchema#string")});
    iri_storage.insert({xsd_langstring_iri_id, std::make_unique<IRIBackend>("http://www.w3.org/1999/02/22-rdf-syntax-ns#langString")});
}
std::pair<LiteralBackend *, ID> ResourceManager::lookup_or_insert_literal(LiteralBackend literal) {
    auto found = literal_storage_reverse.find(literal);
    if (found == literal_storage_reverse.end()) {
        ID id{next_literal_id++};
        auto literal_ptr = std::make_unique<LiteralBackend>(std::move(literal));
        found = literal_storage_reverse.insert(found, {literal_ptr.get(), id});
        literal_storage.insert({id, std::move(literal_ptr)});
    }
    return *found;
}
std::pair<IRIBackend *, ID> ResourceManager::lookup_or_insert_iri(IRIBackend iri) {
    auto found = iri_storage_reverse.find(iri);
    if (found == iri_storage_reverse.end()) {
        ID id{next_iri_id++};
        auto iri_ptr = std::make_unique<IRIBackend>(std::move(iri));
        found = iri_storage_reverse.insert(found, {iri_ptr.get(), id});
        iri_storage.insert({id, std::move(iri_ptr)});
    }
    return *found;
}
std::pair<BNodeBackend *, ID> ResourceManager::lookup_or_insert_bnode(BNodeBackend bnode) {
    auto found = bnode_storage_reverse.find(bnode);
    if (found == bnode_storage_reverse.end()) {
        ID id{next_bnode_id++};
        auto bnode_ptr = std::make_unique<BNodeBackend>(std::move(bnode));
        found = bnode_storage_reverse.insert(found, {bnode_ptr.get(), id});
        bnode_storage.insert({id, std::move(bnode_ptr)});
    }
    return *found;
}
std::pair<VariableBackend *, ID> ResourceManager::lookup_or_insert_variable(VariableBackend variable) {
    auto found = variable_storage_reverse.find(variable);
    if (found == variable_storage_reverse.end()) {
        ID id{next_variable_id++};
        auto variable_ptr = std::make_unique<VariableBackend>(std::move(variable));
        found = variable_storage_reverse.insert(found, {variable_ptr.get(), id});
        variable_storage.insert({id, std::move(variable_ptr)});
    }
    return *found;
}
std::pair<LiteralBackend *, ID> ResourceManager::getLiteral(const std::string &lexical_form) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, xsd_string_iri_id});
}
std::pair<LiteralBackend *, ID> ResourceManager::getTypedLiteral(const std::string &lexical_form, const std::string &datatype) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, lookup_or_insert_iri(IRIBackend{datatype}).second});
}
std::pair<LiteralBackend *, ID> ResourceManager::getLangLiteral(const std::string &lexical_form, const std::string &lang) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, xsd_langstring_iri_id, lang});
}
std::pair<IRIBackend *, ID> ResourceManager::getIRI(const std::string &iri) {
    // TODO: normalize?
    return lookup_or_insert_iri(IRIBackend{iri});
}
std::pair<BNodeBackend *, ID> ResourceManager::getBnode(const std::string &identifier) {
    return lookup_or_insert_bnode(BNodeBackend{identifier});
}
ID ResourceManager::lookup_id(const void *const ptr, const RDFNodeType node_type) {
    switch (node_type) {
        case RDFNodeType::IRI:
            return literal_storage_reverse.at((LiteralBackend *) ptr);
        case RDFNodeType::BNode:
            return bnode_storage_reverse.at((BNodeBackend *) ptr);
        case RDFNodeType::Literal:
            return literal_storage_reverse.at((LiteralBackend *) ptr);
        case RDFNodeType::Variable:
            return variable_storage_reverse.at((VariableBackend *) ptr);
    }
    return ID{};
}
std::pair<VariableBackend *, ID> ResourceManager::getVariable(const std::string &identifier, bool anonymous) {
    return lookup_or_insert_variable(VariableBackend{identifier, anonymous});
}
ResourceManager &ResourceManager::default_instance() {
    static ResourceManager instance{};
    return instance;
}
}  // namespace rdf4cpp::rdf::graph::node_manager