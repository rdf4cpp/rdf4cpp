#ifndef RDF4CPP_STATICVEC_HPP
#define RDF4CPP_STATICVEC_HPP

#include <array>
#include <cstddef>
#include <stdexcept>

namespace rdf4cpp::rdf::storage::util {

/**
 * A vector-like type with a capacity
 * that is bounded by a compiletime constant.
 * The data is stored in the StaticVec itself and not allocated on the heap.
 *
 * @tparam T element_type
 * @tparam MAX_SIZE upper bound for the capacity (the vec cannot grow past this)
 */
template<typename T, size_t MAX_SIZE>
struct StaticVec {
private:
    using storage_t = std::array<T, MAX_SIZE>;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type &;
    using const_reference = value_type const &;
    using pointer = value_type *;
    using const_pointer = value_type const *;

    using iterator = typename storage_t::iterator;
    using const_iterator = typename storage_t::const_iterator;
    using reverse_iterator = typename storage_t::reverse_iterator;
    using const_reverse_iterator = typename storage_t::const_reverse_iterator;

private:
    storage_t storage;
    size_t cur_size = 0;

public:
    constexpr StaticVec() = default;

    constexpr StaticVec(std::initializer_list<T> const inits)
            : cur_size{inits.size()}
    {
        assert(inits.size() <= MAX_SIZE);
        std::copy(inits.begin(), inits.end(), this->storage.begin());
    }

    [[nodiscard]] constexpr size_type size() const noexcept {
        return this->cur_size;
    }

    [[nodiscard]] constexpr size_type max_size() const noexcept {
        return MAX_SIZE;
    }

    [[nodiscard]] constexpr size_type capacity() const noexcept {
        return MAX_SIZE;
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return this->cur_size == 0;
    }

    [[nodiscard]] constexpr pointer data() noexcept {
        return this->storage.data();
    }

    [[nodiscard]] constexpr const_pointer data() const noexcept {
        return this->storage.data();
    }

    constexpr reference front() noexcept {
        return this->storage.front();
    }

    constexpr const_reference front() const noexcept {
        return this->storage.front();
    }

    constexpr reference back() noexcept {
        return *(this->storage.data() + this->cur_size - 1);
    }

    constexpr const_reference back() const noexcept {
        return *(this->storage.data() + this->cur_size - 1);
    }

    constexpr reference operator[](size_type const pos) noexcept {
        return this->storage[pos];
    }

    constexpr const_reference operator[](size_type const pos) const noexcept {
        return this->storage[pos];
    }

    [[nodiscard]] constexpr reference at(size_type const pos) {
        if (pos >= this->cur_size) {
            throw std::out_of_range{"vec index out of range"};
        }

        return (*this)[pos];
    }

    [[nodiscard]] constexpr const_reference at(size_type const pos) const {
        if (pos >= this->cur_size) {
            throw std::out_of_range{"vec index out of range"};
        }

        return (*this)[pos];
    }

    constexpr iterator begin() noexcept {
        return this->storage.begin();
    }

    constexpr iterator end() noexcept {
        return this->storage.begin() + this->cur_size;
    }

    constexpr const_iterator begin() const noexcept {
        return this->storage.begin();
    }

    constexpr const_iterator end() const noexcept {
        return this->storage.begin() + this->cur_size;
    }


    constexpr reverse_iterator rbegin() noexcept {
        return this->storage.rbegin() - this->cur_size;
    }

    constexpr reverse_iterator rend() noexcept {
        return this->storage.rend();
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return this->storage.rbegin() - this->cur_size;
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return this->storage.rend();
    }

    constexpr void push_back(value_type const &value) {
        if (this->cur_size == MAX_SIZE) {
            throw std::length_error{"vec capacity exhausted"};
        }

        this->storage[this->cur_size++] = value;
    }

    constexpr void push_back(value_type &&value) {
        if (this->cur_size == MAX_SIZE) {
            throw std::length_error{"vec capacity exhausted"};
        }

        this->storage[this->cur_size++] = std::move(value);
    }

    template<typename ...Args>
    constexpr reference emplace_back(Args &&...args) {
        if (this->cur_size == MAX_SIZE) {
            throw std::length_error{"vec capacity exhausted"};
        }

        new (&this->storage[this->cur_size]) value_type{std::forward<Args>(args)...};
        return this->storage[this->cur_size++];
    }
};

} // rdf4cpp::rdf::storage::util

#endif  //RDF4CPP_STATICVEC_HPP
