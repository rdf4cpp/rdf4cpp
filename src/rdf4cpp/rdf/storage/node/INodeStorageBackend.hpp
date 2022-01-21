#ifndef RDF4CPP_INODESTORAGEBACKEND_HPP
#define RDF4CPP_INODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/BNodeBackend.hpp>
#include <rdf4cpp/rdf/storage/node/IRIBackend.hpp>
#include <rdf4cpp/rdf/storage/node/LiteralBackend.hpp>
#include <rdf4cpp/rdf/storage/node/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/VariableBackend.hpp>

#include <cstddef>

namespace rdf4cpp::rdf::storage::node {

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

    [[nodiscard]] virtual NodeID get_string_literal_id(std::string_view lexical_form) = 0;

    [[nodiscard]] virtual NodeID get_typed_literal_id(std::string_view lexical_form, std::string_view datatype) = 0;

    [[nodiscard]] virtual NodeID get_typed_literal_id(std::string_view lexical_form, const NodeID &datatype_id) = 0;

    [[nodiscard]] virtual NodeID get_lang_literal_id(std::string_view lexical_form, std::string_view lang) = 0;

    [[nodiscard]] virtual NodeID get_iri_id(std::string_view iri) = 0;

    [[nodiscard]] virtual NodeID get_variable_id(std::string_view identifier, bool anonymous) = 0;

    [[nodiscard]] virtual NodeID get_bnode_id(std::string_view identifier) = 0;

    [[nodiscard]] virtual IRIBackendHandle get_iri_handle(NodeIDValue id) const = 0;

    [[nodiscard]] virtual LiteralBackendHandle get_literal_handle(NodeIDValue id) const = 0;

    [[nodiscard]] virtual BNodeBackendHandle get_bnode_handle(NodeIDValue id) const = 0;

    [[nodiscard]] virtual VariableBackendHandle get_variable_handle(NodeIDValue id) const = 0;
};


}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_INODESTORAGEBACKEND_HPP
