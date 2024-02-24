#ifndef RDF4CPP_UNSYNCNODETYPESTORAGE_HPP
#define RDF4CPP_UNSYNCNODETYPESTORAGE_HPP

#include <dice/hash.hpp>
#include <dice/sparse-map/sparse_map.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>
#include <rdf4cpp/rdf/storage/node/reference_node_storage/detail/BiDirFlatMap.hpp>

#include <memory>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * Storage for one of the Node Backend types. Includes bidirectional mappings between the Backend type and identifier::NodeID,
 * the mappings are not synchronized and are not intended to be thread-safe.
 * @tparam BackendType_t one of BNodeBackend, IRIBackend, FallbackLiteralBackend, SpecializedLiteralBackend and VariableBackend.
 */
template<typename BackendType_t>
struct UnsyncNodeTypeStorage {
    using Backend = BackendType_t;
    using BackendView = typename Backend::View;
private:
    struct DefaultBackendTypeEqual {
        using is_transparent = void;

        bool operator()(BackendView const &lhs, Backend const &rhs) const noexcept {
            return lhs == BackendView(rhs);
        }

        bool operator()(Backend const &lhs, BackendView const &rhs) const noexcept {
            return BackendView(lhs) == rhs;
        }
    };

    template<typename T>
    struct SelectBackendTypeEqual {
        using type = DefaultBackendTypeEqual;
    };

    template<typename T> requires requires { typename T::Equal; }
    struct SelectBackendTypeEqual<T> {
        using type = typename T::Equal;
    };

    struct DefaultBackendTypeHash {
        using is_transparent = void;

        [[nodiscard]] size_t operator()(BackendView const &x) const noexcept {
            return x.hash();
        }
    };

    template<typename T>
    struct SelectBackendTypeHash {
        using type = DefaultBackendTypeHash;
    };

    template<typename T> requires requires { typename T::Hash; }
    struct SelectBackendTypeHash<T> {
        using type = typename T::Hash;
    };

public:
    using BackendEqual = typename SelectBackendTypeEqual<Backend>::type;
    using BackendHash = typename SelectBackendTypeHash<Backend>::type;
    using BackendId = typename Backend::Id;

    static identifier::NodeID to_node_id(BackendId const id, [[maybe_unused]] BackendView const &view) noexcept {
        if constexpr (requires { Backend::to_node_id(id, view); }) {
            return Backend::to_node_id(id, view);
        } else {
            return id;
        }
    }

    static BackendId to_backend_id(identifier::NodeID const id) noexcept {
        if constexpr (requires { Backend::to_backend_id(id); }) {
            return Backend::to_backend_id(id);
        } else {
            return id;
        }
    }

    detail::BiDirFlatMap<BackendId, Backend, BackendView, BackendHash, BackendEqual> mapping;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_UNSYNCNODETYPESTORAGE_HPP
