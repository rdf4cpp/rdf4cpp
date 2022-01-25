#ifndef RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP
#define RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

#include <rdf4cpp/rdf/storage/node/default_node_storage/BNodeBackend.hpp>
#include <rdf4cpp/rdf/storage/node/default_node_storage/IRIBackend.hpp>
#include <rdf4cpp/rdf/storage/node/default_node_storage/LiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/default_node_storage/VariableBackend.hpp>

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace rdf4cpp::rdf::storage::node::default_node_storage {

// TODO: make Backend Node Types reference countable
class DefaultNodeStorageBackend : public INodeStorageBackend {
public:
    using NodeIDValue = identifier::NodeIDValue;
    using LiteralID = identifier::LiteralID;

private:
    mutable std::shared_mutex literal_mutex_;
    std::map<NodeIDValue, LiteralBackend *, std::less<>> literal_storage;
    std::map<std::unique_ptr<LiteralBackend>, NodeIDValue, std::less<>> literal_storage_reverse;
    mutable std::shared_mutex bnode_mutex_;
    std::map<NodeIDValue, BNodeBackend *, std::less<>> bnode_storage;
    std::map<std::unique_ptr<BNodeBackend>, NodeIDValue, std::less<>> bnode_storage_reverse;
    mutable std::shared_mutex iri_mutex_;
    std::map<NodeIDValue, IRIBackend *, std::less<>> iri_storage;
    std::map<std::unique_ptr<IRIBackend>, NodeIDValue, std::less<>> iri_storage_reverse;
    mutable std::shared_mutex variable_mutex_;
    std::map<NodeIDValue, VariableBackend *, std::less<>> variable_storage;
    std::map<std::unique_ptr<VariableBackend>, NodeIDValue, std::less<>> variable_storage_reverse;

    LiteralID next_literal_id = NodeID::min_literal_id;
    NodeIDValue next_bnode_id = NodeID::min_bnode_id;
    NodeIDValue next_iri_id = NodeID::min_iri_id;
    NodeIDValue next_variable_id = NodeID::min_variable_id;

public:
    // TODO: thread safety
    DefaultNodeStorageBackend();

    ~DefaultNodeStorageBackend() override = default;

    [[nodiscard]] NodeID get_string_literal_id(std::string_view lexical_form) override;

    [[nodiscard]] NodeID get_typed_literal_id(std::string_view lexical_form, std::string_view datatype) override;

    [[nodiscard]] NodeID get_typed_literal_id(std::string_view lexical_form, const NodeID &datatype_id) override;

    [[nodiscard]] NodeID get_lang_literal_id(std::string_view lexical_form, std::string_view lang) override;

    [[nodiscard]] NodeID get_iri_id(std::string_view iri) override;

    [[nodiscard]] NodeID get_variable_id(std::string_view identifier, bool anonymous) override;

    [[nodiscard]] NodeID get_bnode_id(std::string_view identifier) override;

    [[nodiscard]] handle::IRIBackendView get_iri_handle(NodeIDValue id) const override;

    [[nodiscard]] handle::LiteralBackendView get_literal_handle(NodeIDValue id) const override;

    [[nodiscard]] handle::BNodeBackendView get_bnode_handle(NodeIDValue id) const override;

    [[nodiscard]] handle::VariableBackendView get_variable_handle(NodeIDValue id) const override;

private:
    std::pair<LiteralBackend *, NodeID> lookup_or_insert_literal(LiteralBackend literal);

    std::pair<IRIBackend *, NodeID> lookup_or_insert_iri(IRIBackend iri);

    std::pair<BNodeBackend *, NodeID> lookup_or_insert_bnode(BNodeBackend bnode);

    std::pair<VariableBackend *, NodeID> lookup_or_insert_variable(VariableBackend variable);
};

}  // namespace rdf4cpp::rdf::storage::node::default_node_storage
#endif  //RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP
