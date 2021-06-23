#ifndef RDF4CPP_INODESTORAGEBACKEND_H
#define RDF4CPP_INODESTORAGEBACKEND_H

#include <rdf4cpp/rdf/graph/node_storage/BNodeBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/IRIBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/LiteralBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/NodeID.h>
#include <rdf4cpp/rdf/graph/node_storage/VariableBackend.h>

#include <cstddef>

namespace rdf4cpp::rdf::graph::node_storage {

class NodeStorage;

class INodeStorageBackend {
    friend NodeStorage;
    static NodeStorageID register_node_context(INodeStorageBackend *);

protected:
    size_t use_count_ = 1;
    size_t nodes_in_use_ = 0;
    NodeStorageID manager_id;

    void inc_use_count() noexcept;
    void dec_use_count() noexcept;
    void inc_nodes_in_use() noexcept;
    void dec_nodes_in_use() noexcept;

    [[nodiscard]] bool is_unreferenced() const noexcept;

public:
    [[nodiscard]] size_t use_count() const noexcept;

    [[nodiscard]] size_t nodes_in_use() const noexcept;
    INodeStorageBackend();
    virtual ~INodeStorageBackend() = 0;

    virtual std::pair<LiteralBackend *, NodeID> get_string_literal(const std::string &lexical_form) = 0;

    virtual std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const std::string &datatype) = 0;

    virtual std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const NodeID &datatype_id) = 0;

    virtual std::pair<LiteralBackend *, NodeID> get_lang_literal(const std::string &lexical_form, const std::string &lang) = 0;

    virtual std::pair<IRIBackend *, NodeID> get_iri(const std::string &iri) = 0;

    virtual std::pair<VariableBackend *, NodeID> get_variable(const std::string &identifier, bool anonymous) = 0;

    virtual std::pair<BNodeBackend *, NodeID> get_bnode(const std::string &identifier) = 0;

    [[nodiscard]] virtual IRIBackend *lookup_iri(NodeIDValue id) const = 0;

    [[nodiscard]] virtual LiteralBackend *lookup_literal(NodeIDValue id) const = 0;

    [[nodiscard]] virtual BNodeBackend *lookup_bnode(NodeIDValue id) const = 0;

    [[nodiscard]] virtual VariableBackend *lookup_variable(NodeIDValue id) const = 0;
};


}  // namespace rdf4cpp::rdf::graph::node_storage

#endif  //RDF4CPP_INODESTORAGEBACKEND_H
