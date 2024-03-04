#ifndef RDF4CPP_STORAGE_NODESTORAGEVTABLE_HPP
#define RDF4CPP_STORAGE_NODESTORAGEVTABLE_HPP

#include <concepts>

#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/view/BNodeBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/LiteralBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/VariableBackendView.hpp>
#include <rdf4cpp/rdf/storage/node/view/IRIBackendView.hpp>

namespace rdf4cpp::rdf::storage::node {

template<typename NS>
concept NodeStorage = requires (NS ns_mut,
                                NS const ns,
                                identifier::LiteralType const lit_type,
                                view::BNodeBackendView const &bnode_view,
                                view::LiteralBackendView const &lit_view,
                                view::VariableBackendView const &var_view,
                                view::IRIBackendView const &iri_view,
                                identifier::NodeID const node_id) {
    /**
     * @return number of nodes managed by this INodeStorageBackend. Implementations are free
     *      to always report 0.
     */
    { ns.size() } -> std::convertible_to<size_t>;

    /**
     * Requests the removal of unused capacity.
     */
    ns_mut.shrink_to_fit();

    /**
     * Backend for NodeStorage::has_specialized_storage_for(datatype)
     * @param datatype datatype of specialized storage to check for
     * @return whether this implementation has specialized storage for the given datatype
     */
    { NS::has_specialized_storage_for(lit_type) } -> std::convertible_to<bool>;

    /**
      * Backend for NodeStorage::find_or_make_id(view::BNodeBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a BlankNode stored at the implementation of INodeStorageBackend.
      */
    { ns_mut.find_or_make_id(bnode_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_or_make_id(view::IRIBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a IRI stored at the implementation of INodeStorageBackend.
      */
    { ns_mut.find_or_make_id(iri_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_or_make_id(view::LiteralBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a BlankNode stored at the implementation of INodeStorageBackend.
      */
    { ns_mut.find_or_make_id(lit_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_or_make_id(view::VariableBackendView const &)
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a Variable stored at the implementation of INodeStorageBackend.
      */
    { ns_mut.find_or_make_id(var_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_id(view::BNodeBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a BlankNode stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such BlankNode stored.
      */
    { ns.find_id(bnode_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_id(view::IRIBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a IRI stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such IRI stored.
      */
    { ns.find_id(iri_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_id(view::LiteralBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a Literal stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such Literal stored.
      */
    { ns.find_id(lit_view) } -> std::same_as<identifier::NodeID>;

    /**
      * Backend for NodeStorage::find_id(view::VariableBackendView const &) const
      * @param view Describes requested node. Can be expected to be valid.
      * @return identifier::NodeID identifying a Variable stored at the implementation of INodeStorageBackend or a null() identifier::NodeID if there is no such Variable stored.
      */
    { ns.find_id(var_view) } -> std::same_as<identifier::NodeID>;

    /**
     * Backend for NodeStorage::find_iri_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::IRIBackendView describing the requested Node
     */
    { ns.find_iri_backend_view(node_id) } -> std::convertible_to<view::IRIBackendView>;

    /**
     * Backend for NodeStorage::find_literal_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::LiteralBackendView describing the requested Node
     */
    { ns.find_literal_backend_view(node_id) } -> std::convertible_to<view::LiteralBackendView>;

    /**
     * Backend for NodeStorage::find_bnode_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::BNodeBackendView describing the requested Node
     */
    { ns.find_bnode_backend_view(node_id) } -> std::convertible_to<view::BNodeBackendView>;

    /**
     * Backend for NodeStorage::find_variable_backend_view(identifier::NodeID id) const. Throws if no Node for the given identifier::NodeID exists.
     * @param id identifier of the requested Node
     * @return view::VariableBackendView describing the requested Node
     */
    { ns.find_variable_backend_view(node_id) } -> std::convertible_to<view::VariableBackendView>;

    /**
     * Backend for NodeStorage::erase_iri(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    { ns_mut.erase_iri(node_id) } -> std::convertible_to<bool>;

    /**
     * Backend for NodeStorage::erase_literal(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    { ns_mut.erase_literal(node_id) } -> std::convertible_to<bool>;

    /**
     * Backend for NodeStorage::erase_bnode(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    { ns_mut.erase_bnode(node_id) } -> std::convertible_to<bool>;

    /**
     * Backend for NodeStorage::erase_variable(identifier::NodeID id) const. Must throw if not implemented.
     * @param id identifier::NodeID identifying the resource
     * @return if backend of resource was erased
     */
    { ns_mut.erase_variable(node_id) } -> std::convertible_to<bool>;
};

struct NodeStorageVTable {
    size_t (*size)(void const *self) noexcept;
    void (*shrink_to_fit)(void *self);
    bool (*has_specialized_storage_for)(identifier::LiteralType literal_type) noexcept;

    identifier::NodeID (*find_or_make_iri_id)(void *self, view::IRIBackendView const &view);
    identifier::NodeID (*find_or_make_bnode_id)(void *self, view::BNodeBackendView const &view);
    identifier::NodeID (*find_or_make_literal_id)(void *self, view::LiteralBackendView const &view);
    identifier::NodeID (*find_or_make_variable_id)(void *self, view::VariableBackendView const &view);

    identifier::NodeID (*find_iri_id)(void const *self, view::IRIBackendView const &view) noexcept;
    identifier::NodeID (*find_bnode_id)(void const *self, view::BNodeBackendView const &view) noexcept;
    identifier::NodeID (*find_literal_id)(void const *self, view::LiteralBackendView const &view) noexcept;
    identifier::NodeID (*find_variable_id)(void const *self, view::VariableBackendView const &view) noexcept;

    view::IRIBackendView (*find_iri_backend)(void const *self, identifier::NodeID id) noexcept;
    view::BNodeBackendView (*find_bnode_backend)(void const *self, identifier::NodeID id) noexcept;
    view::LiteralBackendView (*find_literal_backend)(void const *self, identifier::NodeID id) noexcept;
    view::VariableBackendView (*find_variable_backend)(void const *self, identifier::NodeID id) noexcept;

    bool (*erase_iri)(void *self, identifier::NodeID id);
    bool (*erase_bnode)(void *self, identifier::NodeID id);
    bool (*erase_literal)(void *self, identifier::NodeID id);
    bool (*erase_variable)(void *self, identifier::NodeID id);

    template<NodeStorage NS>
    static NodeStorageVTable const *get() noexcept {
        static constexpr NodeStorageVTable vtable{
            .size = [](void const *self) noexcept -> size_t {
                return static_cast<NS const *>(self)->size();
            },
            .shrink_to_fit = [](void *self) -> void {
                static_cast<NS *>(self)->shrink_to_fit();
            },
            .has_specialized_storage_for = [](identifier::LiteralType const lit_type) noexcept -> bool {
                return NS::has_specialized_storage_for(lit_type);
            },
            .find_or_make_iri_id = [](void *self, view::IRIBackendView const &view) -> identifier::NodeID {
                return static_cast<NS *>(self)->find_or_make_id(view);
            },
            .find_or_make_bnode_id = [](void *self, view::BNodeBackendView const &view) -> identifier::NodeID {
                return static_cast<NS *>(self)->find_or_make_id(view);
            },
            .find_or_make_literal_id = [](void *self, view::LiteralBackendView const &view) -> identifier::NodeID {
                return static_cast<NS *>(self)->find_or_make_id(view);
            },
            .find_or_make_variable_id = [](void *self, view::VariableBackendView const &view) -> identifier::NodeID {
                return static_cast<NS *>(self)->find_or_make_id(view);
            },
            .find_iri_id = [](void const *self, view::IRIBackendView const &view) noexcept -> identifier::NodeID {
                return static_cast<NS const *>(self)->find_id(view);
            },
            .find_bnode_id = [](void const *self, view::BNodeBackendView const &view) noexcept -> identifier::NodeID {
                return static_cast<NS const *>(self)->find_id(view);
            },
            .find_literal_id = [](void const *self, view::LiteralBackendView const &view) noexcept -> identifier::NodeID {
                return static_cast<NS const *>(self)->find_id(view);
            },
            .find_variable_id = [](void const *self, view::VariableBackendView const &view) noexcept -> identifier::NodeID {
                return static_cast<NS const *>(self)->find_id(view);
            },
            .find_iri_backend = [](void const *self, identifier::NodeID id) noexcept -> view::IRIBackendView {
                return static_cast<NS const *>(self)->find_iri_backend_view(id);
            },
            .find_bnode_backend = [](void const *self, identifier::NodeID id) noexcept -> view::BNodeBackendView {
                return static_cast<NS const *>(self)->find_bnode_backend_view(id);
            },
            .find_literal_backend = [](void const *self, identifier::NodeID id) noexcept -> view::LiteralBackendView {
                return static_cast<NS const *>(self)->find_literal_backend_view(id);
            },
            .find_variable_backend = [](void const *self, identifier::NodeID id) noexcept -> view::VariableBackendView {
                return static_cast<NS const *>(self)->find_variable_backend_view(id);
            },
            .erase_iri = [](void *self, identifier::NodeID id) -> bool {
                return static_cast<NS *>(self)->erase_iri(id);
            },
            .erase_bnode = [](void *self, identifier::NodeID id) -> bool {
                return static_cast<NS *>(self)->erase_bnode(id);
            },
            .erase_literal = [](void *self, identifier::NodeID id) -> bool {
                return static_cast<NS *>(self)->erase_literal(id);
            },
            .erase_variable = [](void *self, identifier::NodeID id) -> bool {
                return static_cast<NS *>(self)->erase_variable(id);
            }
        };

        return &vtable;
    }
};

struct DynNodeStorage {
private:
    void *backend_;
    NodeStorageVTable const *vtable_;

public:
    constexpr DynNodeStorage() noexcept: DynNodeStorage{nullptr} {
    }

    constexpr DynNodeStorage(std::nullptr_t) noexcept : backend_{nullptr},
                                                        vtable_{nullptr} {
    }

    template<NodeStorage NS>
    DynNodeStorage(NS &ns) noexcept : backend_{&ns},
                                      vtable_{NodeStorageVTable::get<NS>()} {
    }

    constexpr DynNodeStorage(void *backend, NodeStorageVTable const *vtable) noexcept : backend_{backend},
                                                                                        vtable_{vtable} {
    }

    template<NodeStorage NS>
    DynNodeStorage(std::unique_ptr<NS> const &ptr) noexcept : DynNodeStorage{*ptr} {
    }

    [[nodiscard]] constexpr void *backend() const noexcept {
        return backend_;
    }

    [[nodiscard]] constexpr NodeStorageVTable const *vtable() const noexcept {
        return vtable_;
    }

    [[nodiscard]] size_t size() const noexcept {
        return vtable_->size(backend_);
    }

    void shrink_to_fit() {
        vtable_->shrink_to_fit(backend_);
    }

    [[nodiscard]] bool has_specialized_storage_for(identifier::LiteralType literal_type) noexcept {
        return vtable_->has_specialized_storage_for(literal_type);
    }

    [[nodiscard]] identifier::NodeID find_or_make_id(view::IRIBackendView const &view) {
        return vtable_->find_or_make_iri_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_or_make_id(view::BNodeBackendView const &view) {
        return vtable_->find_or_make_bnode_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_or_make_id(view::LiteralBackendView const &view) {
        return vtable_->find_or_make_literal_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_or_make_id(view::VariableBackendView const &view) {
        return vtable_->find_or_make_variable_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_id(view::IRIBackendView const &view) const noexcept {
        return vtable_->find_iri_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_id(view::BNodeBackendView const &view) const noexcept {
        return vtable_->find_bnode_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_id(view::LiteralBackendView const &view) const noexcept {
        return vtable_->find_literal_id(backend_, view);
    }

    [[nodiscard]] identifier::NodeID find_id(view::VariableBackendView const &view) const noexcept {
        return vtable_->find_variable_id(backend_, view);
    }

    [[nodiscard]] view::IRIBackendView find_iri_backend(identifier::NodeID id) const noexcept {
        return vtable_->find_iri_backend(backend_, id);
    }

    [[nodiscard]] view::BNodeBackendView find_bnode_backend(identifier::NodeID id) const noexcept {
        return vtable_->find_bnode_backend(backend_, id);
    }

    [[nodiscard]] view::LiteralBackendView find_literal_backend(identifier::NodeID id) const noexcept {
        return vtable_->find_literal_backend(backend_, id);
    }

    [[nodiscard]] view::VariableBackendView find_variable_backend(identifier::NodeID id) const noexcept {
        return vtable_->find_variable_backend(backend_, id);
    }

    bool erase_iri(identifier::NodeID id) {
        return vtable_->erase_iri(backend_, id);
    }

    bool erase_bnode(identifier::NodeID id) {
        return vtable_->erase_bnode(backend_, id);
    }

    bool erase_literal(identifier::NodeID id) {
        return vtable_->erase_literal(backend_, id);
    }

    bool erase_variable(identifier::NodeID id) {
        return vtable_->erase_variable(backend_, id);
    }

    std::strong_ordering operator<=>(DynNodeStorage const &other) const noexcept {
        return backend_ <=> other.backend_;
    }

    bool operator==(DynNodeStorage const &other) const noexcept {
        return backend_ == other.backend_;
    }
};

extern DynNodeStorage default_node_storage;

} // namespace rdf4cpp::rdf::storage::node

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::rdf::storage::node::DynNodeStorage> {
    static inline size_t dice_hash(rdf4cpp::rdf::storage::node::DynNodeStorage const &x) noexcept {
        return dice::hash::dice_hash_templates<Policy>::dice_hash(x.backend());
    }
};

template<>
struct std::hash<rdf4cpp::rdf::storage::node::DynNodeStorage> {
    inline size_t operator()(rdf4cpp::rdf::storage::node::DynNodeStorage const &v) const noexcept {
        return ::dice::hash::dice_hash_templates<::dice::hash::Policies::wyhash>::dice_hash(v);
    }
};

#endif // RDF4CPP_STORAGE_NODESTORAGEVTABLE_HPP
