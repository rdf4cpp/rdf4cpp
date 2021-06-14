#include "NodeManager.h"

namespace rdf4cpp::rdf::graph::node_manager {
NodeManager::NodeManager() {
    // some iri's like xsd:string are there by default
    for (const auto &[id, iri] : NodeID::predefined_iris)
        iri_storage.insert({id, std::make_unique<IRIBackend>(iri)});
}
std::pair<LiteralBackend *, NodeID> NodeManager::lookup_or_insert_literal(LiteralBackend literal) {
    auto found = literal_storage_reverse.find(literal);
    if (found == literal_storage_reverse.end()) {
        NodeID id{next_literal_id++};
        auto literal_ptr = std::make_unique<LiteralBackend>(std::move(literal));
        found = literal_storage_reverse.insert(found, {literal_ptr.get(), id});
        literal_storage.insert({id, std::move(literal_ptr)});
    }
    return *found;
}
std::pair<IRIBackend *, NodeID> NodeManager::lookup_or_insert_iri(IRIBackend iri) {
    auto found = iri_storage_reverse.find(iri);
    if (found == iri_storage_reverse.end()) {
        NodeID id{next_iri_id++};
        auto iri_ptr = std::make_unique<IRIBackend>(std::move(iri));
        found = iri_storage_reverse.insert(found, {iri_ptr.get(), id});
        iri_storage.insert({id, std::move(iri_ptr)});
    }
    return *found;
}
std::pair<BNodeBackend *, NodeID> NodeManager::lookup_or_insert_bnode(BNodeBackend bnode) {
    auto found = bnode_storage_reverse.find(bnode);
    if (found == bnode_storage_reverse.end()) {
        NodeID id{next_bnode_id++};
        auto bnode_ptr = std::make_unique<BNodeBackend>(std::move(bnode));
        found = bnode_storage_reverse.insert(found, {bnode_ptr.get(), id});
        bnode_storage.insert({id, std::move(bnode_ptr)});
    }
    return *found;
}
std::pair<VariableBackend *, NodeID> NodeManager::lookup_or_insert_variable(VariableBackend variable) {
    auto found = variable_storage_reverse.find(variable);
    if (found == variable_storage_reverse.end()) {
        NodeID id{next_variable_id++};
        auto variable_ptr = std::make_unique<VariableBackend>(std::move(variable));
        found = variable_storage_reverse.insert(found, {variable_ptr.get(), id});
        variable_storage.insert({id, std::move(variable_ptr)});
    }
    return *found;
}
std::pair<LiteralBackend *, NodeID> NodeManager::get_string_literal(const std::string &lexical_form) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, NodeID::xsd_string_iri.first});
}
std::pair<LiteralBackend *, NodeID> NodeManager::get_typed_literal(const std::string &lexical_form, const std::string &datatype) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, lookup_or_insert_iri(IRIBackend{datatype}).second});
}
std::pair<LiteralBackend *, NodeID> NodeManager::get_typed_literal(const std::string &lexical_form, const NodeID &datatype_id) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, datatype_id});
}

std::pair<LiteralBackend *, NodeID> NodeManager::get_lang_literal(const std::string &lexical_form, const std::string &lang) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, NodeID::xsd_langstring_iri.first, lang});
}
std::pair<IRIBackend *, NodeID> NodeManager::get_iri(const std::string &iri) {
    // TODO: normalize?
    return lookup_or_insert_iri(IRIBackend{iri});
}
std::pair<BNodeBackend *, NodeID> NodeManager::get_bnode(const std::string &identifier) {
    return lookup_or_insert_bnode(BNodeBackend{identifier});
}
NodeID NodeManager::lookup_id(const void *const ptr, const RDFNodeType node_type) {
    switch (node_type) {
        case RDFNodeType::IRI:
            return iri_storage_reverse.at((IRIBackend *) ptr);
        case RDFNodeType::BNode:
            return bnode_storage_reverse.at((BNodeBackend *) ptr);
        case RDFNodeType::Literal:
            return literal_storage_reverse.at((LiteralBackend *) ptr);
        case RDFNodeType::Variable:
            return variable_storage_reverse.at((VariableBackend *) ptr);
    }
    return NodeID{};
}
std::pair<VariableBackend *, NodeID> NodeManager::get_variable(const std::string &identifier, bool anonymous) {
    return lookup_or_insert_variable(VariableBackend{identifier, anonymous});
}
NodeManager &NodeManager::default_instance() {
    static NodeManager instance{};
    return instance;
}
IRIBackend *NodeManager::lookup_iri(NodeID id) const {
    return iri_storage.at(id).get();
}
LiteralBackend *NodeManager::lookup_literal(NodeID id) const {
    return literal_storage.at(id).get();
}
BNodeBackend *NodeManager::lookup_bnode(NodeID id) const {
    return bnode_storage.at(id).get();
}
VariableBackend *NodeManager::lookup_variable(NodeID id) const {
    return variable_storage.at(id).get();
}

}  // namespace rdf4cpp::rdf::graph::node_manager