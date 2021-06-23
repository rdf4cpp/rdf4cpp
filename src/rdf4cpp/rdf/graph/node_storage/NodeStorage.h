#ifndef RDF4CPP_NODESTORAGE_H
#define RDF4CPP_NODESTORAGE_H

#include <rdf4cpp/rdf/graph/node_storage/BNodeBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/DefaultNodeStorageBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/INodeStorageBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/IRIBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/LiteralBackend.h>
#include <rdf4cpp/rdf/graph/node_storage/NodeID.h>
#include <rdf4cpp/rdf/graph/node_storage/VariableBackend.h>

#include <memory>
#include <mutex>
#include <optional>

namespace rdf4cpp::rdf::graph::node_storage {

class INodeStorageBackend;

class NodeStorage {
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


    ~NodeStorage();

    NodeStorage(NodeStorage &&other) noexcept;
    NodeStorage(const NodeStorage &node_context) noexcept;
    NodeStorage &operator=(const NodeStorage &other) noexcept;
    NodeStorage &operator=(NodeStorage &&other) noexcept;

    [[nodiscard]] size_t use_count() const noexcept;

    [[nodiscard]] size_t nodes_in_use() const noexcept;

    [[nodiscard]] NodeStorageID id() const noexcept;

    std::pair<LiteralBackend *, NodeID> get_string_literal(const std::string &lexical_form);

    std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const std::string &datatype);

    std::pair<LiteralBackend *, NodeID> get_typed_literal(const std::string &lexical_form, const NodeID &datatype_id);

    std::pair<LiteralBackend *, NodeID> get_lang_literal(const std::string &lexical_form, const std::string &lang);

    std::pair<IRIBackend *, NodeID> get_iri(const std::string &iri);

    /**
     *
     * @param identifier name name without _: or ?
     * @param anonymous true if string repr. it starts with ? and false if it starts with ?
     * @return
     */
    std::pair<VariableBackend *, NodeID> get_variable(const std::string &identifier, bool anonymous = false);

    /**
     *
     * @param name name without _:
     * @return
     */
    std::pair<BNodeBackend *, NodeID> get_bnode(const std::string &identifier);

    static IRIBackend *lookup_iri(NodeID id);

    static LiteralBackend *lookup_literal(NodeID id);

    static BNodeBackend *lookup_bnode(NodeID id);

    static VariableBackend *lookup_variable(NodeID id);
};

}  // namespace rdf4cpp::rdf::graph::node_storage

#endif  //RDF4CPP_NODESTORAGE_H
