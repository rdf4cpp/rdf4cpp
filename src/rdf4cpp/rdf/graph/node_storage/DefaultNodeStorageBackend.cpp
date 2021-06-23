#include "DefaultNodeStorageBackend.h"
namespace rdf4cpp::rdf::graph::node_storage {

std::pair<LiteralBackend *, NodeID> DefaultNodeStorageBackend::get_string_literal(const std::string &lexical_form) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, NodeID{manager_id, RDFNodeType::IRI, NodeID::xsd_string_iri.first}});
}
std::pair<LiteralBackend *, NodeID> DefaultNodeStorageBackend::get_typed_literal(const std::string &lexical_form, const std::string &datatype) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, lookup_or_insert_iri(IRIBackend{datatype}).second});
}
std::pair<LiteralBackend *, NodeID> DefaultNodeStorageBackend::get_typed_literal(const std::string &lexical_form, const NodeID &datatype_id) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, datatype_id});
}
std::pair<LiteralBackend *, NodeID> DefaultNodeStorageBackend::get_lang_literal(const std::string &lexical_form, const std::string &lang) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, NodeID{manager_id, RDFNodeType::IRI, NodeID::rdf_langstring_iri.first}, lang});
}
std::pair<IRIBackend *, NodeID> DefaultNodeStorageBackend::get_iri(const std::string &iri) {
    // TODO: normalize?
    return lookup_or_insert_iri(IRIBackend{iri});
}
std::pair<VariableBackend *, NodeID> DefaultNodeStorageBackend::get_variable(const std::string &identifier, bool anonymous) {
    return lookup_or_insert_variable(VariableBackend{identifier, anonymous});
}
std::pair<BNodeBackend *, NodeID> DefaultNodeStorageBackend::get_bnode(const std::string &identifier) {
    return lookup_or_insert_bnode(BNodeBackend{identifier});
}
IRIBackend *DefaultNodeStorageBackend::lookup_iri(NodeIDValue id) const {
    return iri_storage.at(id).get();
}
LiteralBackend *DefaultNodeStorageBackend::lookup_literal(NodeIDValue id) const {
    return literal_storage.at(id).get();
}
BNodeBackend *DefaultNodeStorageBackend::lookup_bnode(NodeIDValue id) const {
    return bnode_storage.at(id).get();
}
VariableBackend *DefaultNodeStorageBackend::lookup_variable(NodeIDValue id) const {
    return variable_storage.at(id).get();
}

std::pair<LiteralBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_literal(LiteralBackend literal) {
    auto found = literal_storage_reverse.find(literal);
    NodeID id;
    if (found == literal_storage_reverse.end()) {
        // TODO make sure node_id literal type is set correctly
        id = {manager_id, RDFNodeType::Literal, next_literal_id++, LiteralType::STRING};
        auto literal_ptr = std::make_unique<LiteralBackend>(std::move(literal));
        found = literal_storage_reverse.insert(found, {literal_ptr.get(), id.node_id()});
        literal_storage.insert({id.node_id(), std::move(literal_ptr)});
    } else {
        id = {manager_id, RDFNodeType::Literal, found->second};
    }

    return {found->first, id};
}

std::pair<IRIBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_iri(IRIBackend iri) {
    auto found = iri_storage_reverse.find(iri);
    NodeID id;
    if (found == iri_storage_reverse.end()) {
        id = {manager_id, RDFNodeType::IRI, next_iri_id++};
        auto iri_ptr = std::make_unique<IRIBackend>(std::move(iri));
        found = iri_storage_reverse.insert(found, {iri_ptr.get(), id.node_id()});
        iri_storage.insert({id.node_id(), std::move(iri_ptr)});
    }
    {
        id = {manager_id, RDFNodeType::IRI, found->second};
    }
    return {found->first, id};
}
std::pair<BNodeBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_bnode(BNodeBackend bnode) {
    auto found = bnode_storage_reverse.find(bnode);
    NodeID id;

    if (found == bnode_storage_reverse.end()) {
        id = {manager_id, RDFNodeType::BNode, next_bnode_id++};

        auto bnode_ptr = std::make_unique<BNodeBackend>(std::move(bnode));
        found = bnode_storage_reverse.insert(found, {bnode_ptr.get(), id.node_id()});
        bnode_storage.insert({id.node_id(), std::move(bnode_ptr)});
    } else {
        id = {manager_id, RDFNodeType::BNode, found->second};
    }

    return {found->first, id};
}
std::pair<VariableBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_variable(VariableBackend variable) {
    auto found = variable_storage_reverse.find(variable);
    NodeID id;
    if (found == variable_storage_reverse.end()) {
        id = {manager_id, RDFNodeType::Variable, next_variable_id++};

        auto variable_ptr = std::make_unique<VariableBackend>(std::move(variable));
        found = variable_storage_reverse.insert(found, {variable_ptr.get(), id.node_id()});
        variable_storage.insert({id.node_id(), std::move(variable_ptr)});
    } else {
        id = {manager_id, RDFNodeType::Variable, found->second};
    }
    return {found->first, id};
}
DefaultNodeStorageBackend::DefaultNodeStorageBackend() : INodeStorageBackend() {
    // TODO: that should be done by (Abstract)NodeContextBackend
    // some iri's like xsd:string are there by default
    for (const auto &[id, iri] : NodeID::predefined_iris)
        iri_storage.insert({id, std::make_unique<IRIBackend>(iri)});
}

}  // namespace rdf4cpp::rdf::graph::node_storage