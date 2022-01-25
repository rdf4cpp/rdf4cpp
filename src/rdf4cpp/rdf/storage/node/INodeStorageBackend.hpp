#ifndef RDF4CPP_INODESTORAGEBACKEND_HPP
#define RDF4CPP_INODESTORAGEBACKEND_HPP

#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <cstddef>

namespace rdf4cpp::rdf::storage::node {

class NodeStorage;

class INodeStorageBackend {
    friend NodeStorage;
    static identifier::NodeStorageID register_node_context(INodeStorageBackend *);

protected:
    size_t use_count_ = 1;
    size_t nodes_in_use_ = 0;
    identifier::NodeStorageID manager_id;

    void inc_use_count() noexcept;
    void dec_use_count() noexcept;
    void inc_nodes_in_use() noexcept;
    void dec_nodes_in_use() noexcept;

    [[nodiscard]] bool is_unreferenced() const noexcept;

public:
    using NodeID = identifier::NodeID;
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

    [[nodiscard]] virtual handle::IRIBackendView get_iri_handle(identifier::NodeIDValue id) const = 0;

    [[nodiscard]] virtual handle::LiteralBackendView get_literal_handle(identifier::NodeIDValue id) const = 0;

    [[nodiscard]] virtual handle::BNodeBackendView get_bnode_handle(identifier::NodeIDValue id) const = 0;

    [[nodiscard]] virtual handle::VariableBackendView get_variable_handle(identifier::NodeIDValue id) const = 0;
};


}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_INODESTORAGEBACKEND_HPP
