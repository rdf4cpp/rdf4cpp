#ifndef RDF4CPP_NODESTORAGE_HPP
#define RDF4CPP_NODESTORAGE_HPP

#include <rdf4cpp/rdf/storage/node/INodeStorageBackend.hpp>
#include <rdf4cpp/rdf/storage/node/default_node_storage/DefaultNodeStorageBackend.hpp>
#include <rdf4cpp/rdf/storage/node/handle/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/IRIBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/handle/VariableBackendView.hpp>
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

    [[nodiscard]] NodeID get_string_literal_id(std::string_view lexical_form);

    [[nodiscard]] NodeID get_typed_literal_id(std::string_view lexical_form, std::string_view datatype);

    [[nodiscard]] NodeID get_typed_literal_id(std::string_view lexical_form, const NodeID &datatype_id);

    [[nodiscard]] NodeID get_lang_literal_id(std::string_view lexical_form, std::string_view lang);

    [[nodiscard]] NodeID get_iri_id(std::string_view iri);

    /**
     * Create or lookup a Variable
     * @param identifier name name without `_:` or `?`
     * @param anonymous true if string repr. it starts with `?` and false if it starts with `?`
     * @return a pointer to the VariableBackend and its NodeID
     */
    [[nodiscard]] NodeID get_variable_id(std::string_view identifier, bool anonymous = false);

    /**
     * Create or lookup a BlankNode
     * @param identifier name without `_:`
     * @return a pointer to the BNodeBackend and its NodeID
     */
    [[nodiscard]] NodeID get_bnode_id(std::string_view identifier);

    [[nodiscard]] static handle::IRIBackendView get_iri_handle(NodeID id);

    [[nodiscard]] static handle::LiteralBackendView get_literal_handle(NodeID id);

    [[nodiscard]] static handle::BNodeBackendView get_bnode_handle(NodeID id);

    [[nodiscard]] static handle::VariableBackendView get_variable_handle(NodeID id);

    bool operator==(const NodeStorage &other) const;
};

}  // namespace rdf4cpp::rdf::storage::node

#endif  //RDF4CPP_NODESTORAGE_HPP
