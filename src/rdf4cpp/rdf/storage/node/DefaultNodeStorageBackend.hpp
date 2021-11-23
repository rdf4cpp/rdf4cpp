#ifndef RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP
#define RDF4CPP_DEFAULTNODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>

#include <map>
#include <memory>
#include <shared_mutex>
#include <mutex>

namespace rdf4cpp::rdf::storage::node {

// TODO: make Backend Node Types reference countable
class DefaultNodeStorageBackend : public INodeStorageBackend {

    mutable std::shared_mutex literal_mutex_;
    std::map<NodeIDValue, std::unique_ptr<LiteralBackend>, std::less<>> literal_storage;
    std::map<LiteralBackend *, NodeIDValue, std::less<>> literal_storage_reverse;
    mutable std::shared_mutex bnode_mutex_;
    std::map<NodeIDValue, std::unique_ptr<BNodeBackend>, std::less<>> bnode_storage;
    std::map<BNodeBackend *, NodeIDValue, std::less<>> bnode_storage_reverse;
    mutable std::shared_mutex iri_mutex_;
    std::map<NodeIDValue, std::unique_ptr<IRIBackend>, std::less<>> iri_storage;
    std::map<IRIBackend *, NodeIDValue, std::less<>> iri_storage_reverse;
    mutable std::shared_mutex variable_mutex_;
    std::map<NodeIDValue, std::unique_ptr<VariableBackend>, std::less<>> variable_storage;
    std::map<VariableBackend *, NodeIDValue, std::less<>> variable_storage_reverse;

    LiteralID next_literal_id = NodeID::min_literal_id;
    NodeIDValue next_bnode_id = NodeID::min_bnode_id;
    NodeIDValue next_iri_id = NodeID::min_iri_id;
    NodeIDValue next_variable_id = NodeID::min_variable_id;

public:
    // TODO: thread safety
    DefaultNodeStorageBackend();

    ~DefaultNodeStorageBackend() override = default;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_string_literal(const std::string &lexical_form) override;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const std::string &datatype) override;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const NodeID &datatype_id) override;

    [[nodiscard]] std::pair<LiteralBackend *, NodeID> get_lang_literal(const std::string &lexical_form, const std::string &lang) override;

    [[nodiscard]] std::pair<IRIBackend *, NodeID> get_iri(const std::string &iri) override;

    [[nodiscard]] std::pair<VariableBackend *, NodeID> get_variable(const std::string &identifier, bool anonymous) override;

    [[nodiscard]] std::pair<BNodeBackend *, NodeID> get_bnode(const std::string &identifier) override;

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
