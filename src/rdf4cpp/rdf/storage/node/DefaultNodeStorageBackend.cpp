#include "DefaultNodeStorageBackend.hpp"

#include <functional>

namespace rdf4cpp::rdf::storage::node {

NodeID DefaultNodeStorageBackend::get_string_literal_id(std::string_view lexical_form) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, NodeID{manager_id, RDFNodeType::IRI, NodeID::xsd_string_iri.first}}).second;
}
NodeID DefaultNodeStorageBackend::get_typed_literal_id(std::string_view lexical_form, std::string_view datatype) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, lookup_or_insert_iri(IRIBackend{datatype}).second}).second;
}
NodeID DefaultNodeStorageBackend::get_typed_literal_id(std::string_view lexical_form, const NodeID &datatype_id) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, datatype_id}).second;
}
NodeID DefaultNodeStorageBackend::get_lang_literal_id(std::string_view lexical_form, std::string_view lang) {
    return lookup_or_insert_literal(LiteralBackend{lexical_form, NodeID{manager_id, RDFNodeType::IRI, NodeID::rdf_langstring_iri.first}, lang}).second;
}
NodeID DefaultNodeStorageBackend::get_iri_id(std::string_view iri) {
    // TODO: normalize?
    return lookup_or_insert_iri(IRIBackend{iri}).second;
}
NodeID DefaultNodeStorageBackend::get_variable_id(std::string_view identifier, bool anonymous) {
    return lookup_or_insert_variable(VariableBackend{identifier, anonymous}).second;
}
NodeID DefaultNodeStorageBackend::get_bnode_id(std::string_view identifier) {
    return lookup_or_insert_bnode(BNodeBackend{identifier}).second;
}
IRIBackendHandle DefaultNodeStorageBackend::get_iri_handle(NodeIDValue id) const {
    std::shared_lock<std::shared_mutex> shared_lock{iri_mutex_};
    return IRIBackendHandle(*iri_storage.at(id));
}
LiteralBackendHandle DefaultNodeStorageBackend::get_literal_handle(NodeIDValue id) const {
    std::shared_lock<std::shared_mutex> shared_lock{literal_mutex_};
    return LiteralBackendHandle(*literal_storage.at(id));
}
BNodeBackendHandle DefaultNodeStorageBackend::get_bnode_handle(NodeIDValue id) const {
    std::shared_lock<std::shared_mutex> shared_lock{bnode_mutex_};

    return BNodeBackendHandle(*bnode_storage.at(id));
}
VariableBackendHandle DefaultNodeStorageBackend::get_variable_handle(NodeIDValue id) const {
    std::shared_lock<std::shared_mutex> shared_lock{variable_mutex_};
    return VariableBackendHandle(*variable_storage.at(id));
}

std::pair<LiteralBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_literal(LiteralBackend literal) {
    std::shared_lock<std::shared_mutex> shared_lock{literal_mutex_};
    auto found = literal_storage_reverse.find(literal);
    NodeID id;
    if (found == literal_storage_reverse.end()) {
        shared_lock.unlock();
        std::unique_lock<std::shared_mutex> unique_lock{literal_mutex_};
        // update found (might have changed in the meantime)
        found = literal_storage_reverse.find(literal);
        if (found == literal_storage_reverse.end()) {
            // TODO make sure node_id literal type is set correctly
            id = {manager_id, RDFNodeType::Literal, next_literal_id++, LiteralType::STRING};
            auto [found2, inserted_successfully] = literal_storage_reverse.emplace(std::make_unique<LiteralBackend>(std::move(literal)), id.node_id());
            assert(inserted_successfully);
            found = found2;
            literal_storage.insert({id.node_id(), found->first.get()});
        } else {
            unique_lock.unlock();
            id = {manager_id, RDFNodeType::Literal, found->second};
        }
    } else {
        shared_lock.unlock();
        id = {manager_id, RDFNodeType::Literal, found->second};
    }

    return {found->first.get(), id};
}

std::pair<IRIBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_iri(IRIBackend iri) {
    std::shared_lock<std::shared_mutex> shared_lock{iri_mutex_};
    auto found = iri_storage_reverse.find(iri);
    NodeID id;
    if (found == iri_storage_reverse.end()) {
        shared_lock.unlock();
        std::unique_lock<std::shared_mutex> unique_lock{iri_mutex_};
        // update found (might have changed in the meantime)
        found = iri_storage_reverse.find(iri);
        if (found == iri_storage_reverse.end()) {
            id = {manager_id, RDFNodeType::IRI, next_iri_id++};
            auto [found2, inserted_successfully] = iri_storage_reverse.emplace(std::make_unique<IRIBackend>(std::move(iri)), id.node_id());
            assert(inserted_successfully);
            found = found2;
            iri_storage.insert({id.node_id(), found->first.get()});
        } else {
            unique_lock.unlock();
            id = {manager_id, RDFNodeType::IRI, found->second};
        }
    } else {
        shared_lock.unlock();
        id = {manager_id, RDFNodeType::IRI, found->second};
    }
    return {found->first.get(), id};
}
std::pair<BNodeBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_bnode(BNodeBackend bnode) {
    std::shared_lock<std::shared_mutex> shared_lock{bnode_mutex_};
    auto found = bnode_storage_reverse.find(bnode);
    NodeID id;

    if (found == bnode_storage_reverse.end()) {
        shared_lock.unlock();
        std::unique_lock<std::shared_mutex> unique_lock{bnode_mutex_};
        // update found (might have changed in the meantime)
        found = bnode_storage_reverse.find(bnode);
        if (found == bnode_storage_reverse.end()) {
            id = {manager_id, RDFNodeType::BNode, next_bnode_id++};
            auto [found2, inserted_successfully] = bnode_storage_reverse.emplace(std::make_unique<BNodeBackend>(std::move(bnode)), id.node_id());
            assert(inserted_successfully);
            found = found2;
            bnode_storage.insert({id.node_id(), found->first.get()});
        } else {
            unique_lock.unlock();
            id = {manager_id, RDFNodeType::BNode, found->second};
        }
    } else {
        shared_lock.unlock();
        id = {manager_id, RDFNodeType::BNode, found->second};
    }

    return {found->first.get(), id};
}
std::pair<VariableBackend *, NodeID> DefaultNodeStorageBackend::lookup_or_insert_variable(VariableBackend variable) {
    std::shared_lock<std::shared_mutex> shared_lock{variable_mutex_};
    auto found = variable_storage_reverse.find(variable);
    NodeID id;
    if (found == variable_storage_reverse.end()) {
        shared_lock.unlock();
        std::unique_lock<std::shared_mutex> unique_lock{variable_mutex_};
        // update found (might have changed in the meantime)
        found = variable_storage_reverse.find(variable);
        if (found == variable_storage_reverse.end()) {
            id = {manager_id, RDFNodeType::Variable, next_variable_id++};
            auto [found2, inserted_successfully] = variable_storage_reverse.emplace(std::make_unique<VariableBackend>(std::move(variable)), id.node_id());
            assert(inserted_successfully);
            found = found2;
            variable_storage.insert({id.node_id(), found->first.get()});
        } else {
            unique_lock.unlock();
            id = {manager_id, RDFNodeType::Variable, found->second};
        }
    } else {
        shared_lock.unlock();
        id = {manager_id, RDFNodeType::Variable, found->second};
    }
    return {found->first.get(), id};
}
DefaultNodeStorageBackend::DefaultNodeStorageBackend() : INodeStorageBackend() {
    // TODO: that should be done by (Abstract)NodeContextBackend
    // some iri's like xsd:string are there by default
    for (const auto &[id, iri] : NodeID::predefined_iris) {
        auto [iter, inserted_successfully] = iri_storage_reverse.emplace(std::make_unique<IRIBackend>(iri), id);
        assert(inserted_successfully);
        iri_storage.insert({id, iter->first.get()});
    }
}
}  // namespace rdf4cpp::rdf::storage::node