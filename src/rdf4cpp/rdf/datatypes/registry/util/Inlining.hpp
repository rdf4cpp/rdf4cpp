#ifndef RDF4CPP_REGISTRY_UTIL_INLINING_HPP
#define RDF4CPP_REGISTRY_UTIL_INLINING_HPP

namespace rdf4cpp::rdf::datatypes::registry::util {

/**
 * @brief packs the bits of a value into the lower bits of a bigger type
 * @tparam P bigger type to pack the bits into
 * @tparam T smaller type to be packed
 * @param value value to be packed
 * @return the packed value
 */
template<typename P, typename T>
constexpr P pack(T value) noexcept {
    static_assert(sizeof(P) > sizeof(T));
    static_assert(sizeof(P) % sizeof(T) == 0);

    union {
        std::array<T, sizeof(P) / sizeof(T)> source;
        P target;
    } reinterpret{ .source = {value} };

    return reinterpret.target;
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
    static_assert(sizeof(P) > sizeof(T));
    static_assert(sizeof(P) % sizeof(T) == 0);

    union {
        P source;
        std::array<T, sizeof(P) / sizeof(T)> target;
    } reinterpret{ .source = packed_value };

    return reinterpret.target[0];
}

} // namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_UTIL_INLINING_HPP
