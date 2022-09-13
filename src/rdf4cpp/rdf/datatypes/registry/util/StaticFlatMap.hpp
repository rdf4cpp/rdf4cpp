#ifndef RDF4CPP_REGISTRY_UTIL_STATICFLATMAP_HPP
#define RDF4CPP_REGISTRY_UTIL_STATICFLATMAP_HPP

#include <cstddef>
#include <algorithm>
#include <array>
#include <utility>
#include <cassert>

namespace rdf4cpp::rdf::datatypes::registry::util {

/**
 * A map with fixed/static (maximum) capacity that can be used in constexpr contexts
 *
 * @tparam Key key type
 * @tparam Value mapped type
 * @tparam capacity maximum number of mappings that can be contained in the map
 */
template<typename Key, typename Value, size_t capacity>
struct StaticFlatMap {
private:
    using storage_t = std::array<std::pair<Key, Value>, capacity>;

    size_t max_init;
    storage_t storage;

    constexpr void sort() noexcept {
        std::ranges::sort(this->storage.begin(), this->storage.begin() + this->max_init, {},
                          [](auto const &entry) {
                              return entry.first;
                          });
    }

public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<Key, Value>;
    using const_reference = value_type const &;
    using const_pointer = value_type const *;

    using size_type = typename storage_t::size_type;
    using difference_type = typename storage_t::difference_type;
    using const_iterator = typename storage_t::const_iterator;

    constexpr StaticFlatMap(std::initializer_list<std::pair<Key, Value>> const inits) {
        assert(inits.size() <= capacity);

        std::copy(inits.begin(), inits.end(), this->storage.begin());

        this->max_init = inits.size();
        this->sort();
    }

    [[nodiscard]] constexpr size_type size() const noexcept {
        return this->max_init;
    }

    [[nodiscard]] constexpr size_type max_size() const noexcept {
        return capacity;
    }

    constexpr const_iterator begin() const noexcept {
        return this->storage.begin();
    }

    constexpr const_iterator end() const noexcept {
        return this->storage.begin() + this->max_init;
    }

    constexpr const_iterator find(key_type const &search_key) const noexcept {
        auto const res = std::ranges::lower_bound(this->begin(), this->end(), search_key, {},
                                            [](auto const &entry) {
                                                return entry.first;
                                            });

        if (res != this->end() && res->first == search_key) {
            return res;
        } else {
            return this->end();
        }
    }

    constexpr bool contains(key_type const &search_key) const noexcept {
        return this->find(search_key) != this->end();
    }

    constexpr mapped_type const &operator[](key_type const &key) const noexcept {
        auto it = this->find(key);
        return it->second;
    }
};
} // rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_UTIL_STATICFLATMAP_HPP
