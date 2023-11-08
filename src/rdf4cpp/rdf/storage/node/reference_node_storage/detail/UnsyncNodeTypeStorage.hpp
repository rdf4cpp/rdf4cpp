#ifndef RDF4CPP_UNSYNCNODETYPESTORAGE_HPP
#define RDF4CPP_UNSYNCNODETYPESTORAGE_HPP

#include <dice/hash.hpp>
#include <dice/sparse-map/sparse_map.hpp>
#include <rdf4cpp/rdf/storage/node/identifier/NodeID.hpp>

#include <memory>
#include <shared_mutex>


namespace rdf4cpp::rdf::storage::node::reference_node_storage {

/**
 * Storage for one of the Node Backend types. Includes a shared mutex to synchronize access and bidirectional mappings between the Backend type and identifier::NodeID.
 * @tparam BackendType_t one of BNodeBackend, IRIBackend, FallbackLiteralBackend, SpecializedLiteralBackend and VariableBackend.
 */
template<class BackendType_t>
struct UnsyncNodeTypeStorage {
    using Backend = BackendType_t;
    using BackendView = typename Backend::View;
private:
    struct DefaultBackendTypeEqual {
        using is_transparent = void;

        bool operator()(Backend const *lhs, Backend const *rhs) const noexcept {
            return lhs == rhs;
        }
        bool operator()(BackendView const &lhs, Backend const *rhs) const noexcept {
            return lhs == BackendView(*rhs);
        }

        bool operator()(Backend const *lhs, BackendView const &rhs) const noexcept {
            return BackendView(*lhs) == rhs;
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

        [[nodiscard]] size_t operator()(Backend const *x) const noexcept {
            return x->hash;
        }
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

    struct NodeIDHash {
        [[nodiscard]] size_t operator()(identifier::NodeID const &x) const noexcept {
            return dice::hash::dice_hash_templates<dice::hash::Policies::wyhash>::dice_hash(x.value());
        }
    };

    dice::sparse_map::sparse_map<identifier::NodeID, std::unique_ptr<Backend>, NodeIDHash> id2data;
    dice::sparse_map::sparse_map<Backend *, identifier::NodeID, BackendHash, BackendEqual> data2id;
};

}  // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif  //RDF4CPP_UNSYNCNODETYPESTORAGE_HPP
