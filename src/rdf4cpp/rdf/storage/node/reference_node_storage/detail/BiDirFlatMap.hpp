#ifndef RDF4CPP_RDF_REFERENCENODESTORAGE_BIDIRFLATMAP_HPP
#define RDF4CPP_RDF_REFERENCENODESTORAGE_BIDIRFLATMAP_HPP

#include <dice/sparse-map/sparse_set.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage {

template<typename Id, typename Value, typename View, typename Hash, typename Equal, typename Allocator = std::allocator<Value>>
struct BiDirFlatMap {
    using id_type = Id;
    using mapped_type = Value;
    using view_type = View;
    using size_type = size_t;
    using allocator_type = Allocator;

private:
    struct backward_key_type {
        size_t hash_;
        id_type id_;
    };

    struct backward_key_equal {
        using is_transparent = void;

        std::vector<mapped_type *> const *reference_;
        [[no_unique_address]] Equal eq_;

        bool operator()(backward_key_type const &a, backward_key_type const &b) const noexcept {
            return a.id_ == b.id_;
        }

        bool operator()(backward_key_type const &a, view_type const &b) const noexcept {
            auto const *ref = (*reference_)[to_index(a.id_)];
            if (ref == nullptr) {
                return false;
            }

            return eq_(*ref, b);
        }

        bool operator()(view_type const &a, backward_key_type const &b) const noexcept {
            auto const *ref = (*reference_)[to_index(b.id_)];
            if (ref == nullptr) {
                return false;
            }

            return eq_(a, *ref);
        }
    };

    struct backward_hasher {
        using is_transparent = void;

        [[no_unique_address]] Hash hash_;

        size_t operator()(backward_key_type const &a) const noexcept {
            return a.hash_;
        }

        size_t operator()(View const &a) const noexcept {
            return hash_(a);
        }
    };

    using forward_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<mapped_type *>;
    using backward_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<backward_key_type>;

    std::vector<mapped_type *, forward_allocator_type> forward_;
    dice::sparse_map::sparse_set<backward_key_type, backward_hasher, backward_key_equal, backward_allocator_type> backward_;
    [[no_unique_address]] allocator_type alloc_;

    [[nodiscard]] static size_type to_index(id_type id) noexcept {
        return static_cast<size_type>(id) - 1;
    }

    [[nodiscard]] static id_type to_id(size_type ix) noexcept {
        return static_cast<id_type>(ix + 1);
    }

public:
    explicit BiDirFlatMap(Hash const &hash = Hash{},
                          Equal const &equal = Equal{}) noexcept : forward_{},
                                                                   backward_{0, backward_hasher{hash}, backward_key_equal{&forward_, equal}} {
    }

    // deleted because type is self-referential
    BiDirFlatMap(BiDirFlatMap const &) = delete;
    BiDirFlatMap(BiDirFlatMap &&) = delete;
    BiDirFlatMap &operator=(BiDirFlatMap const &) = delete;
    BiDirFlatMap &operator=(BiDirFlatMap &&) = delete;

    ~BiDirFlatMap() noexcept {
        for (auto ptr : forward_) {
            if (ptr != nullptr) {
                std::allocator_traits<allocator_type>::destroy(alloc_, ptr);
                std::allocator_traits<allocator_type>::deallocate(alloc_, ptr, 1);
            }
        }
    }

    [[nodiscard]] size_type size() const noexcept {
        return forward_.size();
    }

    [[nodiscard]] mapped_type const *lookup_value(id_type const id) const noexcept {
        if (id == id_type{}) [[unlikely]] {
            return nullptr;
        }

        auto const ix = to_index(id);
        if (ix >= forward_.size()) {
            return nullptr;
        }

        return forward_[ix];
    }

    [[nodiscard]] id_type lookup_id(view_type const &view) const noexcept {
        auto it = backward_.find(view);
        if (it == backward_.end()) {
            return Id{};
        }

        return it->id_;
    }

    void reserve(id_type const min_id) noexcept {
        forward_.resize(to_index(min_id));
    }

    [[nodiscard]] id_type insert_assume_not_present(view_type const &view) noexcept {
        assert(lookup_id(view) == Id{});

        auto const assigned_id = to_id(forward_.size());

        mapped_type *backend = std::allocator_traits<allocator_type>::allocate(alloc_, 1);
        std::allocator_traits<allocator_type>::construct(alloc_, backend, view);
        forward_.push_back(backend);

        auto const h = backward_.hash_function().hash_(view);
        backward_.emplace(h, assigned_id);

        return assigned_id;
    }

    void insert_assume_not_present_at(view_type const &view, id_type requested_id) noexcept {
        assert(lookup_id(view) == Id{});

        auto const lookup_ix = to_index(requested_id);
        assert(lookup_ix < forward_.size());
        assert(forward_[lookup_ix] == nullptr);

        mapped_type *backend = std::allocator_traits<allocator_type>::allocate(alloc_, 1);
        std::allocator_traits<allocator_type>::construct(alloc_, backend, view);
        forward_[lookup_ix] = backend;

        auto const h = backward_.hash_function().hash_(view);
        backward_.emplace(h, requested_id);
    }

    void erase_assume_present(id_type id) noexcept {
        assert(lookup_value(id) != nullptr);

        auto &value = forward_[to_index(id)];
        auto const view = static_cast<view_type>(*value);

        backward_.erase(view);

        std::allocator_traits<allocator_type>::destroy(alloc_, value);
        std::allocator_traits<allocator_type>::deallocate(alloc_, value, 1);
        value = nullptr;
    }
};

} // namespace rdf4cpp::rdf::storage::node::reference_node_storage

#endif // RDF4CPP_RDF_REFERENCENODESTORAGE_BIDIRFLATMAP_HPP
