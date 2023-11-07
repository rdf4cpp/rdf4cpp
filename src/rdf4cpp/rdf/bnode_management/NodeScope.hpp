#ifndef RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
#define RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP

#include <rdf4cpp/rdf/bnode_management/INodeScope.hpp>
#include <rdf4cpp/rdf/bnode_management/NodeGenerator.hpp>
#include <rdf4cpp/rdf/bnode_management/reference_backends/scope/ReferenceNodeScope.hpp>
#include <rdf4cpp/rdf/storage/node/NodeStorage.hpp>

#include <string_view>

namespace rdf4cpp::rdf::util {

namespace identifier {

struct NodeScopeID {
    static constexpr size_t width = 16;
    using underlying_type = uint16_t;

private:
    underlying_type raw_ : width;

public:
    constexpr NodeScopeID() = default;
    explicit constexpr NodeScopeID(underlying_type raw) : raw_{raw} {}

    [[nodiscard]] constexpr underlying_type to_underlying() const noexcept {
        return raw_;
    }

    constexpr auto operator<=>(NodeScopeID const &) const noexcept = default;
};

} // namespace identifier

namespace node_scope_detail {

struct ControlBlock {
    std::atomic<INodeScope *> backend{nullptr};
    std::atomic<size_t> refcount{0};
    std::atomic<size_t> generation{0};
};

inline constexpr identifier::NodeScopeID INVALID_BACKEND_INDEX{static_cast<uint16_t>(-1)};

} // namespace node_scope_detail


struct WeakNodeScope;

/**
 * A scope for generating Nodes that remembers the nodes it generates.
 * Uses an IIdGenerator and INodeScope internally to perform its function.
 *
 * Instances of this type cannot be constructed directly, use NodeGenerator::scope instead.
 */
struct NodeScope {
    using NodeStorage = storage::node::NodeStorage;

private:
    friend struct WeakNodeScope;
    friend struct std::hash<NodeScope>;

    static inline constinit std::array<node_scope_detail::ControlBlock, (1 << 16) - 1> node_scope_instances_{};

    identifier::NodeScopeID backend_index_;
    INodeScope *cached_backend_ptr_;

    /**
     * Retrives the control block at the slot with the given id
     * @param id id of the slot
     * @return the corresponding control block
     */
    [[nodiscard]] inline static node_scope_detail::ControlBlock &get_slot(identifier::NodeScopeID id) noexcept {
        assert(id != node_scope_detail::INVALID_BACKEND_INDEX);
        return NodeScope::node_scope_instances_[static_cast<size_t>(id.to_underlying())];
    }

    /**
     * Increases the reference count of the backend instance pointed to by
     * this NodeScope.
     *
     * @safety This function must only be called if you can guarantee
     *      that the reference count of this backend cannot reach zero while calling this function.
     *      I.e. Something like NodeScope{id}.increase_refcount(); is inherently undefined behaviour.
     */
    void increase_refcount() noexcept;

    /**
     * Decreases the reference count of the backend instance pointed to by
     * this NodeScope.
     *
     * @safety This function must only be called on destruction or similar events (like assignment)
     *      calling it in any other scenario is undefined behaviour as it could destroy the backend
     *      other or this NodeScope is referring to.
     */
    void decrease_refcount() noexcept;

    explicit NodeScope(identifier::NodeScopeID backend_index) noexcept;
    NodeScope(identifier::NodeScopeID backend_index, INodeScope *cached_backend_ptr) noexcept;

public:
    NodeScope(NodeScope const &other) noexcept;
    NodeScope(NodeScope &&other) noexcept;
    NodeScope &operator=(NodeScope const &other) noexcept;
    NodeScope &operator=(NodeScope &&other) noexcept;

    ~NodeScope();

public:

    /**
     * Registers a INodeScope.
     *
     * @param backend_instance instance to be registered
     * @return an NodeStorage encapsulating backend_instance
     * @throws std::invalid_argument if a nullptr is provided
     * @throws std::logic_error if the provided backend is already registered
     * @throws std::length_error if the maximum number of node scopes was exceeded
     * @safety This function takes ownership of the backend, do _not_ delete it manually or use it directly in any way
     *      after calling this function.
     */
    static NodeScope register_backend(INodeScope *&&backend_instance);

    /**
     * Registers a INodeScope at a specific ID
     *
     * @param id id to assign the instance
     * @param backend_instance instance to be registered
     * @return an NodeStorage encapsulating backend_instance
     * @throws std::invalid_argument if a nullptr is provided
     * @throws std::logic_error if the provided backend is already registered
     * @throws std::length_error if the maximum number of node scopes was exceeded
     * @safety This function takes ownership of the backend, do _not_ delete it manually or use it directly in any way
     *      after calling this function.
     */
    static NodeScope register_backend_at(identifier::NodeScopeID id, INodeScope *&&backend_instance);

    /**
     * Create a new instance using reference_backends::scope::ReferenceNodeScope
     * @return NodeScope backed by reference_backends::scope::ReferenceNodeScope
     */
    static NodeScope new_instance();

    template<typename BackendImpl, typename ...Args> requires std::derived_from<BackendImpl, INodeScope>
    static NodeScope new_instance(Args &&...args) {
        return register_backend(new BackendImpl{std::forward<Args>(args)...});
    }

    template<typename BackendImpl, typename ...Args> requires std::derived_from<BackendImpl, INodeScope>
    static NodeScope new_instance_at(identifier::NodeScopeID id, Args &&...args) {
        return register_backend_at(id, new BackendImpl{std::forward<Args>(args)...});
    }

    /**
     * Retrieve NodeScope for the given ID. If no such NodeStorage exists or the backend is already dead, std::nullopt is returned.
     * @param id NodeScope id
     * @return optionally the NodeScope if it is alive
     */
    static std::optional<NodeScope> lookup_instance(identifier::NodeScopeID id) noexcept;

    /**
     * NodeScope does instance reference counting. If no instances of a NodeScope exist anymore its backend is destroyed.
     * For the default_instance an additional instance is kept so that it is not destroyed if the there are no application held instance left.
     * @return current number of instances of this NodeScope
     * @safey this function is inherently racy as the reference count can even change between fetching it and looking at the value
     *      do _not_ use it to check the reference count for _any_ kind of synchronization.
     */
    [[nodiscard]] size_t ref_count() const noexcept;

    /**
     * @return Identifier of this NodeScope
     */
    [[nodiscard]] identifier::NodeScopeID id() const noexcept;

    /**
     * @return a WeakNodeStorage pointing to the same backend as this NodeScope
     */
    [[nodiscard]] WeakNodeScope downgrade() const noexcept;

    /**
     * Generates a new node without remembering it. This behaves identically to NodeGenerator::generate_node
     * @param node_storage where to place the generated node
     * @return the generated node
     */
    [[nodiscard]] Node generate_node(NodeGenerator &generator = NodeGenerator::default_instance(), NodeStorage &node_storage = NodeStorage::default_instance()) const;

    /**
     * Try to fetch a previously generated (and remembered) node by using the previously used label.
     * @param label the label that was used initially to generate the searched-for node
     * @return the node if it was found or a null-node if this scope does not remember the given label
     */
    [[nodiscard]] Node try_get_node(std::string_view label) const noexcept;

    /**
     * Try to fetch a previously generated (and remembered) node by using the previously used label.
     * If the scope remembers no such label, a new node is generated and remembered.
     *
     * @param label the label of the previous node/the label of the new node that will be generated
     * @param node_storage where to place the newly generated node
     * @return either the found node or the newly generated node
     */
    [[nodiscard]] Node get_or_generate_node(std::string_view label, NodeGenerator &generator = NodeGenerator::default_instance(), NodeStorage &node_storage = NodeStorage::default_instance());

    [[nodiscard]] std::optional<std::string_view> try_get_label(Node const &node) const noexcept;

    /**
     * @return whether this and other refer to the same backends
     */
    bool operator==(NodeScope const &other) const noexcept;

    /**
     * @return whether this and other do _not_ refer to the same backends
     */
    bool operator!=(NodeScope const &other) const noexcept;
};

struct WeakNodeScope {
private:
    friend struct NodeScope;
    friend struct std::hash<WeakNodeScope>;

    identifier::NodeScopeID backend_index_;
    size_t generation_;

    WeakNodeScope(identifier::NodeScopeID backend_index, size_t generation) noexcept;

public:
    /**
     * @return Identifier of this NodeScope
     */
    [[nodiscard]] identifier::NodeScopeID id() const noexcept;

    /**
     * Tries to upgrade this WeakNodeScope into a NodeScope.
     * This will only succeed if the corresponding INodeScope backend is still alive.
     *
     * @return a NodeScope pointing to the same backend as this, if the backend is still alive, otherwise nullopt
     */
    [[nodiscard]] std::optional<NodeScope> try_upgrade() const noexcept;

    /**
     * Tries to upgrade this WeakNodeScope into a NodeScope.
     * This will only succeed if the corresponding INodeScope is still alive.
     * This function throws on failure to upgrade.
     *
     * @return a NodeScope pointing to the same backend as this, if the backend is still alive
     * @throws std::runtime_error on upgrade failure
     */
    [[nodiscard]] NodeScope upgrade() const;

    /**
     * @return whether this and other are referring to the same backend
     */
    bool operator==(WeakNodeScope const &other) const noexcept = default;

    /**
     * @return whether this and other are _not_ referring to the same backend
     */
    bool operator!=(WeakNodeScope const &other) const noexcept = default;
};

}  //namespace rdf4cpp::rdf::util

template<>
struct std::hash<rdf4cpp::rdf::util::NodeScope> {
    size_t operator()(rdf4cpp::rdf::util::NodeScope const &scope) const noexcept;
};

template<>
struct std::hash<rdf4cpp::rdf::util::WeakNodeScope> {
    size_t operator()(rdf4cpp::rdf::util::WeakNodeScope const &scope) const noexcept;
};

#endif  //RDF4CPP_RDF_UTIL_BLANKNODEIDSCOPE_HPP
