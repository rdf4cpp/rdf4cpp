#ifndef RDF4CPP_RDF_REFERENCENODESTORAGE_BIDIRFLATMAP_HPP
#define RDF4CPP_RDF_REFERENCENODESTORAGE_BIDIRFLATMAP_HPP

#include <dice/sparse-map/sparse_set.hpp>

namespace rdf4cpp::rdf::storage::node::reference_node_storage::detail {

/**
 * A bidirectional map from Id to Value
 *
 * @tparam Id id type
 * @tparam Value stored value type
 * @tparam View view of Value
 * @tparam Hash hash for View
 * @tparam Equal equality for View and Value
 * @tparam Allocator allocator
 */
template<typename Id, typename Value, typename View, typename Hash = std::hash<View>, typename Equal = std::equal_to<>, typename Allocator = std::allocator<Value>>
struct BiDirFlatMap {
    using id_type = Id;
    using mapped_type = Value;
    using view_type = View;
    using size_type = size_t;
    using allocator_type = Allocator;
    using hasher = Hash;
    using key_equal = Equal;

private:
    using forward_value_type = std::optional<mapped_type>;
    using forward_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<forward_value_type>;
    using forward_type = std::vector<forward_value_type, forward_allocator_type>;

    struct backward_key_type {
        size_t hash; // hash of the Value being looked up
        id_type id;  // id into forward_
    };

    struct backward_key_equal {
        using is_transparent = void;

        using forward_const_pointer = typename std::allocator_traits<allocator_type>::template rebind_traits<forward_type>::const_pointer;

        forward_const_pointer forward_ptr; // pointer to forward_ to be able to check for equality between View and Value
        [[no_unique_address]] Equal eq;

        bool operator()(backward_key_type const &a, backward_key_type const &b) const noexcept {
            return a.id == b.id;
        }

        bool operator()(backward_key_type const &a, view_type const &b) const noexcept {
            auto const &ref = (*forward_ptr)[to_index(a.id)];
            if (!ref.has_value()) {
                return false;
            }

            return eq(*ref, b);
        }

        bool operator()(view_type const &a, backward_key_type const &b) const noexcept {
            auto const &ref = (*forward_ptr)[to_index(b.id)];
            if (!ref.has_value()) {
                return false;
            }

            return eq(a, *ref);
        }
    };

    struct backward_hasher {
        using is_transparent = void;

        [[no_unique_address]] Hash hash;

        size_t operator()(backward_key_type const &a) const noexcept {
            return a.hash;
        }

        size_t operator()(view_type const &a) const noexcept {
            return hash(a);
        }
    };

    using backward_allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<backward_key_type>;
    using backward_type = dice::sparse_map::sparse_set<backward_key_type, backward_hasher, backward_key_equal, backward_allocator_type>;

    forward_type forward_;   // forward_[id] stores Value for Id id
    backward_type backward_; // backward_[view] stores Id into forward_
    [[no_unique_address]] allocator_type alloc_;

    [[nodiscard]] static constexpr size_type to_index(id_type id) noexcept {
        return static_cast<size_type>(id) - 1;
    }

    [[nodiscard]] static constexpr id_type to_id(size_type ix) noexcept {
        return static_cast<id_type>(ix + 1);
    }

public:
    explicit BiDirFlatMap(hasher const &hash = hasher{},
                          key_equal const &equal = key_equal{},
                          allocator_type const &alloc = allocator_type{}) noexcept : forward_{alloc},
                                                                                     backward_{0, backward_hasher{hash}, backward_key_equal{&forward_, equal}, alloc},
                                                                                     alloc_{alloc} {
    }

    explicit BiDirFlatMap(allocator_type const &alloc) noexcept : BiDirFlatMap{hasher{}, key_equal{}, alloc} {
    }

    // deleted because type is self-referential
    BiDirFlatMap(BiDirFlatMap const &) = delete;
    BiDirFlatMap(BiDirFlatMap &&) = delete;
    BiDirFlatMap &operator=(BiDirFlatMap const &) = delete;
    BiDirFlatMap &operator=(BiDirFlatMap &&) = delete;

    [[nodiscard]] size_type size() const noexcept {
        return forward_.size();
    }

    void shrink_to_fit() {
        forward_.shrink_to_fit();
        // TODO backward_?
    }

    [[nodiscard]] std::optional<view_type> lookup_value(id_type const id) const noexcept {
        if (id == id_type{}) [[unlikely]] {
            return std::nullopt;
        }

        auto const ix = to_index(id);
        if (ix >= forward_.size() || !forward_[ix].has_value()) {
            return std::nullopt;
        }

        return static_cast<view_type>(*forward_[ix]);
    }

    [[nodiscard]] id_type lookup_id(view_type const &view) const noexcept {
        auto it = backward_.find(view);
        if (it == backward_.end()) {
            return Id{};
        }

        return it->id;
    }

    void reserve(id_type const min_id) {
        forward_.resize(to_index(min_id));
    }

    [[nodiscard]] id_type insert_assume_not_present(view_type const &view) {
        assert(lookup_id(view) == Id{});

        auto const assigned_id = to_id(forward_.size());

        forward_.push_back(std::make_obj_using_allocator<mapped_type>(alloc_, view));
        auto const h = backward_.hash_function().hash(view);
        backward_.emplace(h, assigned_id);

        return assigned_id;
    }

    void insert_assume_not_present_at(view_type const &view, id_type requested_id) {
        assert(lookup_id(view) == Id{});

        auto const lookup_ix = to_index(requested_id);
        assert(lookup_ix < forward_.size());
        assert(!forward_[lookup_ix].has_value());

        forward_[lookup_ix] = std::make_obj_using_allocator<mapped_type>(alloc_, view);

        auto const h = backward_.hash_function().hash(view);
        backward_.emplace(h, requested_id);
    }

    void erase_assume_present(id_type id) {
        assert(lookup_value(id).has_value());

        auto &value = forward_[to_index(id)];
        auto const view = static_cast<view_type>(*value);

        backward_.erase(view);
        value.reset();
    }
};

} // namespace rdf4cpp::rdf::storage::node::reference_node_storage::detail

#endif // RDF4CPP_RDF_REFERENCENODESTORAGE_BIDIRFLATMAP_HPP
