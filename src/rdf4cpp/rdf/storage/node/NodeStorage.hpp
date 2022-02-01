#ifndef RDF4CPP_NODESTORAGE_HPP
#define RDF4CPP_NODESTORAGE_HPP

#include <rdf4cpp/rdf/storage/node/default_node_storage/DefaultNodeStorageBackend.hpp>
#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeBackendHandle.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <memory>
#include <mutex>
#include <optional>

namespace rdf4cpp::rdf::storage::node {

class INodeStorageBackend;

class NodeStorage {
public:
    using NodeID = identifier::NodeID;
    using NodeStorageID = identifier::NodeStorageID;

private:
    friend INodeStorageBackend;
    INodeStorageBackend *backend_{};

    NodeStorage() = default;

    explicit NodeStorage(INodeStorageBackend *backend) : backend_(backend) {}
    // TODO: encapsulate to make sure that the default instance is clean destructed on termination.
    static inline std::array<INodeStorageBackend *, 1024> node_context_instances{};
    static inline std::once_flag default_init_once_flag;
    static inline NodeStorageID default_node_context_id;
    static NodeStorage default_instance_;

    static INodeStorageBackend *lookup_backend_instance(NodeStorageID id);

public:
    static NodeStorage &primary_instance();

    static void primary_instance(const NodeStorage &node_context);

    template<typename BackendImpl, typename... Args>
    static inline NodeStorage new_instance(Args... args) {
        return NodeStorage(new BackendImpl(args...));
    }

    static NodeStorage new_instance();

    static std::optional<NodeStorage> lookup_instance(NodeStorageID id);

    static NodeStorage register_backend(INodeStorageBackend *backend_instance);

    static void unregister_backend(INodeStorageBackend *backend_instance);


    ~NodeStorage();

    NodeStorage(NodeStorage &&other) noexcept;
    NodeStorage(const NodeStorage &node_context) noexcept;
    NodeStorage &operator=(const NodeStorage &other) noexcept;
    NodeStorage &operator=(NodeStorage &&other) noexcept;

    [[nodiscard]] size_t use_count() const noexcept;

    [[nodiscard]] size_t nodes_in_use() const noexcept;

    [[nodiscard]] identifier::NodeStorageID id() const noexcept;

    [[nodiscard]] identifier::NodeID find_or_make_id(handle::BNodeBackendView const &view) noexcept;
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::IRIBackendView const &view) noexcept;
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::LiteralBackendView const &view) noexcept;
    [[nodiscard]] identifier::NodeID find_or_make_id(handle::VariableBackendView const &view) noexcept;

    [[nodiscard]] identifier::NodeID find_id(handle::BNodeBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeID find_id(handle::IRIBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeID find_id(handle::LiteralBackendView const &view) const noexcept;
    [[nodiscard]] identifier::NodeID find_id(handle::VariableBackendView const &view) const noexcept;

    [[nodiscard]] handle::IRIBackendView find_iri_backend_view(identifier::NodeID id) const;
    [[nodiscard]] handle::LiteralBackendView find_literal_backend_view(identifier::NodeID id) const;
    [[nodiscard]] handle::BNodeBackendView find_bnode_backend_view(identifier::NodeID id) const;
    [[nodiscard]] handle::VariableBackendView find_variable_backend_view(identifier::NodeID id) const;
    [[nodiscard]] static handle::IRIBackendView find_iri_backend_view(identifier::NodeBackendHandle handle);
    [[nodiscard]] static handle::LiteralBackendView find_literal_backend_view(identifier::NodeBackendHandle handle);
    [[nodiscard]] static handle::BNodeBackendView find_bnode_backend_view(identifier::NodeBackendHandle handle);
    [[nodiscard]] static handle::VariableBackendView find_variable_backend_view(identifier::NodeBackendHandle handle);

    bool erase_iri(identifier::NodeID id) const;
    bool erase_literal(identifier::NodeID id) const;
    bool erase_bnode(identifier::NodeID id) const;
    bool erase_variable(identifier::NodeID id) const;
    static bool erase_iri(identifier::NodeBackendHandle handle);
    static bool erase_literal(identifier::NodeBackendHandle handle);
    static bool erase_bnode(identifier::NodeBackendHandle handle);
    static bool erase_variable(identifier::NodeBackendHandle handle);

    bool operator==(const NodeStorage &other) const;
};


}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_NODESTORAGE_HPP
