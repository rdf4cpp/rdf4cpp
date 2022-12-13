#ifndef RDF4CPP_REGISTRY_UTIL_INLINING_HPP
#define RDF4CPP_REGISTRY_UTIL_INLINING_HPP

namespace rdf4cpp::rdf::datatypes::registry::util {

namespace packing_detail {

    template<typename Source, typename Target>
    union __attribute__((__packed__)) PackingHelper {
        static constexpr size_t source_padding = sizeof(Target) > sizeof(Source) ? sizeof(Target) - sizeof(Source) : 0;
        static constexpr size_t target_padding = sizeof(Source) > sizeof(Target) ? sizeof(Source) - sizeof(Target) : 0;

        struct __attribute__((__packed__)) {
            Source value;
            std::array<std::byte, source_padding> pad{};
        } source;

        struct __attribute__((__packed__)) {
            Target value;
            std::array<std::byte, target_padding> pad{};
        } target;
    };

    template<size_t N>
    constexpr bool is_padding_empty(std::array<std::byte, N> const &pad) noexcept {
        return std::all_of(pad.begin(), pad.end(), [](auto const byte) { return byte == std::byte{0}; });
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
    packing_detail::PackingHelper<T, P> reinterpret{ .source = { .value = value } };
    assert(packing_detail::is_padding_empty(reinterpret.target.pad));
    return reinterpret.target.value;
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
    packing_detail::PackingHelper<P, T> reinterpret{ .source = { .value = packed_value } };
    assert(packing_detail::is_padding_empty(reinterpret.target.pad));
    return reinterpret.target.value;
}

} // namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_UTIL_INLINING_HPP
