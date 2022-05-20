#ifndef RDF4CPP_ROBIN_HOOD_HASH_HPP
#define RDF4CPP_ROBIN_HOOD_HASH_HPP

#include <array>
#include <cstring>
#include <memory>

namespace rdf4cpp::rdf::storage::util::robin_hood {

namespace detail {

template<typename T>
inline T unaligned_load(void const *ptr) noexcept {
    // using memcpy so we don't get into unaligned load problems.
    // compiler should optimize this very well anyways.
    T t;
    std::memcpy(&t, ptr, sizeof(T));
    return t;
}
}  // namespace detail
inline size_t hash_bytes(void const *ptr, size_t len) noexcept {
    static constexpr uint64_t m = UINT64_C(0xc6a4a7935bd1e995);
    static constexpr uint64_t seed = UINT64_C(0xe17a1465);
    static constexpr unsigned int r = 47;

    auto const *const data64 = static_cast<uint64_t const *>(ptr);
    uint64_t h = seed ^ (len * m);

    size_t const n_blocks = len / 8;
    for (size_t i = 0; i < n_blocks; ++i) {
        auto k = detail::unaligned_load<uint64_t>(data64 + i);

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    auto const *const data8 = reinterpret_cast<uint8_t const *>(data64 + n_blocks);
    switch (len & 7U) {
        case 7:
            h ^= static_cast<uint64_t>(data8[6]) << 48U;
            [[fallthrough]];  // FALLTHROUGH
        case 6:
            h ^= static_cast<uint64_t>(data8[5]) << 40U;
            [[fallthrough]];  // FALLTHROUGH
        case 5:
            h ^= static_cast<uint64_t>(data8[4]) << 32U;
            [[fallthrough]];  // FALLTHROUGH
        case 4:
            h ^= static_cast<uint64_t>(data8[3]) << 24U;
            [[fallthrough]];  // FALLTHROUGH
        case 3:
            h ^= static_cast<uint64_t>(data8[2]) << 16U;
            [[fallthrough]];  // FALLTHROUGH
        case 2:
            h ^= static_cast<uint64_t>(data8[1]) << 8U;
            [[fallthrough]];  // FALLTHROUGH
        case 1:
            h ^= static_cast<uint64_t>(data8[0]);
            h *= m;
            [[fallthrough]];  // FALLTHROUGH
        default:
            break;
    }

    h ^= h >> r;

    // not doing the final step here, because this will be done by keyToIdx anyways
    // h *= m;
    // h ^= h >> r;
    return static_cast<size_t>(h);
}

inline size_t hash_int(uint64_t x) noexcept {
    // tried lots of different hashes, let's stick with murmurhash3. It's simple, fast, well tested,
    // and doesn't need any special 128bit operations.
    x ^= x >> 33U;
    x *= UINT64_C(0xff51afd7ed558ccd);
    x ^= x >> 33U;

    // not doing the final step here, because this will be done by keyToIdx anyways
    // x *= UINT64_C(0xc4ceb9fe1a85ec53);
    // x ^= x >> 33U;
    return static_cast<size_t>(x);
}

// A thin wrapper around std::hash, performing an additional simple mixing step of the result.
template<typename T, typename Enable = void>
struct hash : public std::hash<T> {
    size_t operator()(T const &obj) const
            noexcept(noexcept(std::declval<std::hash<T>>().operator()(std::declval<T const &>()))) {
        // call base hash
        auto result = std::hash<T>::operator()(obj);
        // return mixed of that, to be save against identity has
        return hash_int(static_cast<uintptr_t>(result));
    }
};

template<typename CharT>
struct hash<std::basic_string<CharT>> {
    size_t operator()(std::basic_string<CharT> const &str) const noexcept {
        return hash_bytes(str.data(), sizeof(CharT) * str.size());
    }
};

template<typename CharT>
struct hash<std::basic_string_view<CharT>> {
    size_t operator()(std::basic_string_view<CharT> const &sv) const noexcept {
        return hash_bytes(sv.data(), sizeof(CharT) * sv.size());
    }
};

template<typename Tp, std::size_t Nm>
struct hash<std::array<Tp, Nm>> {
    size_t operator()(std::array<Tp, Nm> const &sv) const noexcept {
        return hash_bytes(sv.data(), sizeof(Tp) * Nm);
    }
};

template<class T>
struct hash<T *> {
    size_t operator()(T *ptr) const noexcept {
        return hash_int(reinterpret_cast<uintptr_t>(ptr));
    }
};

template<class T>
struct hash<std::unique_ptr<T>> {
    size_t operator()(std::unique_ptr<T> const &ptr) const noexcept {
        return hash_int(reinterpret_cast<uintptr_t>(ptr.get()));
    }
};

template<class T>
struct hash<std::shared_ptr<T>> {
    size_t operator()(std::shared_ptr<T> const &ptr) const noexcept {
        return hash_int(reinterpret_cast<uintptr_t>(ptr.get()));
    }
};

template<typename Enum>
struct hash<Enum, typename std::enable_if<std::is_enum<Enum>::value>::type> {
    size_t operator()(Enum e) const noexcept {
        using Underlying = typename std::underlying_type<Enum>::type;
        return hash<Underlying>{}(static_cast<Underlying>(e));
    }
};

#define ROBIN_HOOD_HASH_INT(T)                           \
    template<>                                           \
    struct hash<T> {                                     \
        size_t operator()(T const &obj) const noexcept { \
            return hash_int(static_cast<uint64_t>(obj)); \
        }                                                \
    }
}  // namespace rdf4cpp::rdf::storage::util::robin_hood
#endif  //RDF4CPP_ROBIN_HOOD_HASH_HPP
