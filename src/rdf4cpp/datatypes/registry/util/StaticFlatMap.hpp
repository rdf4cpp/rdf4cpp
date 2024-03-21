#ifndef RDF4CPP_REGISTRY_UTIL_STATICFLATMAP_HPP
#define RDF4CPP_REGISTRY_UTIL_STATICFLATMAP_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace rdf4cpp::datatypes::registry::util {

/**
 * A map with fixed/static (maximum) capacity that can be used in constexpr contexts
 * @note aggregate initialization will not deduplicate entries in the map. It is unspecified
 *   which of the elements (with same key) will be found by find or operator[].
 *
 * @tparam Key key type
 * @tparam Value mapped type
 * @tparam capacity number of mappings that are contained in the map
 */
template<typename Key, typename Value, size_t capacity>
struct StaticFlatMap {
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using const_reference = value_type const &;
    using const_pointer = value_type const *;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using const_iterator = value_type const *;

    using storage_t = std::array<value_type, capacity>;
    storage_t storage;

    [[nodiscard]] constexpr size_type size() const noexcept {
        return capacity;
    }

    [[nodiscard]] constexpr size_type max_size() const noexcept {
        return capacity;
    }

    constexpr const_iterator begin() const noexcept {
        return this->storage.begin();
    }

    constexpr const_iterator end() const noexcept {
        return this->storage.end();
    }

    constexpr const_iterator find(key_type const &search_key) const noexcept {
        return std::ranges::find(this->begin(), this->end(), search_key, [](auto const &entry) {
            return entry.first;
        });
    }

    constexpr bool contains(key_type const &search_key) const noexcept {
        return this->find(search_key) != this->end();
    }

    /**
     * Finds the entry with the key `key`.
     *
     * @note this function can and will exhibit ub during runtime when the map does not contain the search key.
     *   Be careful when calling this function during runtime.
     *   In constexpr contexts this is no problem since compilers are required to report ub there.
     */
    constexpr mapped_type const &operator[](key_type const &key) const noexcept {
        auto it = this->find(key);
        return it->second;
    }
};

template<typename ...Ps>
StaticFlatMap(Ps &&...) -> StaticFlatMap<std::common_type_t<typename std::remove_cvref_t<Ps>::first_type...>, std::common_type_t<typename std::remove_cvref_t<Ps>::second_type...>, sizeof...(Ps)>;

} // rdf4cpp::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_UTIL_STATICFLATMAP_HPP
