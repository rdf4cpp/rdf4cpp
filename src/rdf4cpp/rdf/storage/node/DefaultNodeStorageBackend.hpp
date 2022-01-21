#ifndef RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP
#define RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace rdf4cpp::rdf::storage::node {

// TODO: make Backend Node Types reference countable
class DefaultNodeStorageBackend : public INodeStorageBackend {

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

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_string_literal(std::string_view lexical_form) override;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_typed_literal(std::string_view lexical_form, std::string_view datatype) override;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_typed_literal(std::string_view lexical_form, const NodeID &datatype_id) override;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_lang_literal(std::string_view lexical_form, std::string_view lang) override;

    [[nodiscard]] std::pair<IRIBackend *, NodeID> get_iri(std::string_view iri) override;

    [[nodiscard]] std::pair<VariableBackend *, NodeID> get_variable(std::string_view identifier, bool anonymous) override;

    [[nodiscard]] std::pair<BNodeBackend *, NodeID> get_bnode(std::string_view identifier) override;

    [[nodiscard]] IRIBackend *lookup_iri(NodeIDValue id) const override;

    [[nodiscard]] LiteralBackend *lookup_literal(NodeIDValue id) const override;

    [[nodiscard]] BNodeBackend *lookup_bnode(NodeIDValue id) const override;

    [[nodiscard]] VariableBackend *lookup_variable(NodeIDValue id) const override;

private:
    std::pair<LiteralBackend *, NodeID> lookup_or_insert_literal(LiteralBackend literal);

    std::pair<IRIBackend *, NodeID> lookup_or_insert_iri(IRIBackend iri);

    std::pair<BNodeBackend *, NodeID> lookup_or_insert_bnode(BNodeBackend bnode);

    std::pair<VariableBackend *, NodeID> lookup_or_insert_variable(VariableBackend variable);
};

}  // namespace rdf4cpp::rdf::storage::node
#endif  //RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP
