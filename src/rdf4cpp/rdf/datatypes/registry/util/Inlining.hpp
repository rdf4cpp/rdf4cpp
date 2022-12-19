#ifndef RDF4CPP_REGISTRY_UTIL_INLINING_HPP
#define RDF4CPP_REGISTRY_UTIL_INLINING_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>

namespace rdf4cpp::rdf::datatypes::registry::util {

namespace packing_detail {

    template<typename P, typename T>
    union __attribute__((__packed__)) PackingHelper {
        static_assert(sizeof(T) <= sizeof(P), "Packed into type must be at least as large as type to pack");
        static constexpr size_t value_padding = sizeof(P) - sizeof(T);

        struct __attribute__((__packed__)) {
            T value;
            std::array<std::byte, value_padding> pad{};
        } unpacked_value;

        P packed_value;
    };

    template<size_t bit, typename T>
    constexpr bool no_information_in_bits_after(T value) noexcept {
        if constexpr (bit >= sizeof(T) * 8) {
            return true;
        } else {
            auto const indicator_bit = (value >> bit) & 1;
            auto const mask = ~((indicator_bit << (bit + 1)) - 1);

            return value == (value | mask);
        }
    }

} // namespace packing_detail

/**
 * @brief packs the bits of a value into the lower bits of a bigger type
 * @tparam P bigger type to pack the bits into
 * @tparam T smaller type to be packed
 * @param value value to be packed
 * @return the packed value
 */
template<typename P, typename T>
constexpr P pack(T value) noexcept {
    static_assert(sizeof(T) <= sizeof(P));

    packing_detail::PackingHelper<P, T> reinterpret{ .unpacked_value = { .value = value } };
    return reinterpret.packed_value;
}

/**
 * @brief reverse operation of pack
 * @tparam T smaller type to be unpacked
 * @tparam P bigger type that the value of T was packed into
 * @param packed_value the packed value to unpack
 * @return the unpacked value
 */
template<typename T, typename P>
constexpr T unpack(P packed_value) noexcept {
    static_assert(sizeof(T) <= sizeof(P));

    packing_detail::PackingHelper<P, T> reinterpret{ .packed_value = packed_value };
    return reinterpret.unpacked_value.value;
}

template<typename P, size_t bits, std::signed_integral T>
constexpr P pack_signed(T value) noexcept {
    // bits after boundary cannot hold any information
    assert(packing_detail::no_information_in_bits_after<bits - 1>(value));

    // clear bits without information
    if constexpr (sizeof(T) * 8 > bits) {
        P const clear_mask = (P{1} << bits) - 1;
        value &= clear_mask;
    }

    return pack<P>(value);
}

template<std::signed_integral T, size_t bits, typename P>
constexpr T unpack_signed(P packed_value) noexcept {
    auto const sign_ext_shift_amt = sizeof(T) * 8 - bits;

    // smear bit that indicates remaining/truncated bits to the left
    // note: depends on arithmetic right shift (implementation defined)
    return unpack<T>(packed_value) << sign_ext_shift_amt >> sign_ext_shift_amt;
}

template<typename P, size_t bits, std::integral T>
constexpr std::optional<P> try_pack_integral(T value) noexcept {
    if constexpr (sizeof(T) * 8 <= bits) {
        return pack<P>(value);
    } else if constexpr (std::unsigned_integral<T>) {
        if (value >= (T{1} << bits)) [[unlikely]] {
            return std::nullopt;
        }

        return pack<P>(value);
    } else {
        if (auto const boundary = T{1} << (bits - 1); value >= boundary || value < -boundary) [[unlikely]] {
            return std::nullopt;
        }

        return pack_signed<P, bits>(value);
    }
}

template<std::integral T, size_t bits, typename P>
constexpr T unpack_integral(P packed_value) noexcept {
    if constexpr (sizeof(T) * 8 <= bits || std::unsigned_integral<T>) {
        return unpack<T>(packed_value);
    } else {
        return unpack_signed<T, bits>(packed_value);
    }
}

} // namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_UTIL_INLINING_HPP
