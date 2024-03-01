#ifndef RDF4CPP_RDF_REFERENCENODESTORAGE_INDEXFREELIST_HPP
#define RDF4CPP_RDF_REFERENCENODESTORAGE_INDEXFREELIST_HPP

#include <cstddef>
#include <ranges>
#include <vector>

namespace rdf4cpp::rdf::storage::node::reference_node_storage::detail {
/**
 * A freelist for indices inside a vector.
 *
 * @tparam BitmapType type used for the freelist elements
 * @tparam Allocator allocator
 */
template<typename BitmapType = size_t, typename Allocator = std::allocator<size_t>>
struct IndexFreeList {
    using bitmap_type = BitmapType;
    using allocator_type = Allocator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

private:
    static constexpr size_t bitmap_bits = sizeof(bitmap_type) * 8;

    size_t next_free_ix_; //< bit index to the next free bit
    std::vector<bitmap_type, allocator_type> occupied_bitmap_; //< nth bit of the allocation stores if slot n is occupied

    /**
     * Decomposes a bit index into slot index and bit offset
     */
    static constexpr std::pair<size_type, size_type> decompose(size_type const bit_ix) noexcept {
        return std::make_pair(bit_ix / bitmap_bits, bit_ix % bitmap_bits);
    }

    /**
     * Composes slot index and bit offset into a bit index
     */
    static constexpr size_type compose(size_type const slot_ix, size_type const offset) noexcept {
        return (slot_ix * 64) + offset;
    }

    /**
     * Sets the bit at slot_ix, offset to 1
     */
    void set_bit(size_type const slot_ix, size_type const offset) noexcept {
        occupied_bitmap_[slot_ix] |= 1ul << offset;
    }

    /**
     * Sets the bit at slot_ix, offset to 0
     */
    void unset_bit(size_type const slot_ix, size_type const offset) noexcept {
        occupied_bitmap_[slot_ix] &= ~(1ul << offset);
    }

    /**
     * Searches the next free bit after the given slot_ix, offset pair
     *
     * @precondition all slots before slot_ix, offset are occupied
     *
     * @param slot_ix slot index
     * @param offset bit offset into slot
     * @return index of the next free bit
     */
    [[nodiscard]] size_type search_next_free(size_type slot_ix, size_type const offset) const noexcept {
        size_type new_offset = std::countr_one(occupied_bitmap_[slot_ix]);
        assert(new_offset >= offset);

        if (new_offset < bitmap_bits) {
            return compose(slot_ix, new_offset);
        }

        slot_ix += 1;
        while (slot_ix < occupied_bitmap_.size()) {
            new_offset = std::countr_one(occupied_bitmap_[slot_ix]);
            if (new_offset < bitmap_bits) {
                return compose(slot_ix, new_offset);
            }

            slot_ix += 1;
        }

        return compose(occupied_bitmap_.size(), 0);
    }

public:
    explicit IndexFreeList(allocator_type const &alloc = allocator_type{}) noexcept : next_free_ix_{0},
                                                                                      occupied_bitmap_{alloc} {
    }

    /**
     * Requests the removal of unused capacity
     */
    void shrink_to_fit() {
        auto const last_occupied = std::find_if(occupied_bitmap_.rbegin(), occupied_bitmap_.rend(), [](auto const bitmap) noexcept {
            return bitmap != 0;
        });

        auto const unused_size = std::distance(occupied_bitmap_.rbegin(), last_occupied);
        occupied_bitmap_.resize(occupied_bitmap_.size() - unused_size);
    }

    /**
     * Marks all slots up-to (not-including) the given index as occupied
     */
    void occupy_until(size_type const ix) {
        static constexpr bitmap_type all_ones = std::numeric_limits<bitmap_type>::max();

        auto const [slot_ix, offset] = decompose(ix);
        if (slot_ix >= occupied_bitmap_.size()) {
            occupied_bitmap_.resize(0);
            occupied_bitmap_.resize(slot_ix + 1, all_ones);
            occupied_bitmap_[slot_ix] = (1ul << offset) - 1;
        } else {
            std::fill_n(occupied_bitmap_.begin(), slot_ix, all_ones);
            occupied_bitmap_[slot_ix] |= (1ul << offset) - 1;
        }

        next_free_ix_ = search_next_free(slot_ix, offset);
    }

    /**
     * Searches the next non-occupied index and returns it, while also marking it as occupied.
     */
    [[nodiscard]] size_type occupy_next_available() {
        auto const [slot_ix, offset] = decompose(next_free_ix_);
        if (slot_ix >= occupied_bitmap_.size()) {
            occupied_bitmap_.push_back(0);
        }

        set_bit(slot_ix, offset);
        return std::exchange(next_free_ix_, search_next_free(slot_ix, offset));
    }

    /**
     * Marks index ix as not occupied
     */
    void vacate(size_type const ix) noexcept {
        auto const [slot_ix, offset] = decompose(ix);
        unset_bit(slot_ix, offset);

        if (ix < next_free_ix_) {
            next_free_ix_ = ix;
        }
    }
};

} // namespace rdf4cpp::rdf::storage::node::reference_node_storage::detail

#endif // RDF4CPP_RDF_REFERENCENODESTORAGE_INDEXFREELIST_HPP
