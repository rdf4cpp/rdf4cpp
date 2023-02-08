#ifndef RDF4CPP_RDF_UTIL_SHAREDPTR_HPP
#define RDF4CPP_RDF_UTIL_SHAREDPTR_HPP

#include <atomic>
#include <cassert>
#include <utility>
#include <optional>

#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::util {

namespace shared_ptr_detail {

/**
 * The control block of SharedPtr/WeakPtr
 */
struct ControlBlock {
    /**
     * Number of SharedPtrs (pointing to this control block) currently alive
     * Values greater than 0 keep the data (not stored here) alive.
     */
    std::atomic<size_t> strong;

    /**
     * Number of WeakPtrs (pointing to this control block) currently alive.
     * Values greater than 0 keep this control block alive.
     */
    std::atomic<size_t> weak;
};

}  //namespace shared_ptr_detail

template<typename T>
struct WeakPtr;

template<typename T>
struct SharedPtr;

/**
 * Equivalent to std::enable_shared_from_this<T>
 */
template<typename T>
struct EnableSharedFromThis {
private:
    template<typename U>
    friend struct SharedPtr;

    WeakPtr<T> mutable weak;

    void set_owner(WeakPtr<T> &&owner) const noexcept {
        this->weak = std::move(owner);
    }
public:
    SharedPtr<T> shared_from_this() const noexcept {
        return this->weak.upgrade();
    }

    WeakPtr<T> weak_from_this() const noexcept {
        return this->weak;
    }
};

/**
 * Shared pointer implementation, that exposes ptr equality and hash on WeakPtrs.
 * This implementation is inspired by Rust's std::sync::Arc and boost's boost::shared_ptr.
 * @tparam T type of object to manage
 */
template<typename T>
struct SharedPtr {
    using element_type = T;
    using weak_type = WeakPtr<T>;

private:
    using control_block_type = shared_ptr_detail::ControlBlock;

    template<typename U>
    friend struct SharedPtr;

    template<typename U>
    friend struct WeakPtr;

    friend struct EnableSharedFromThis<T>;

    control_block_type *control_block = nullptr;
    element_type *data = nullptr;

    void drop() noexcept(std::is_nothrow_destructible_v<T>) {
        if (this->control_block == nullptr) {
            return;
        }

        // Release-store to synchronize deletion with
        // weak upgrade
        if (this->control_block->strong.fetch_sub(1, std::memory_order_release) != 1) {
            return;
        }

        // Prevent reordering of deletion of data with use of data
        // www.boost.org/doc/libs/1_55_0/doc/html/atomic/usage_examples.html
        std::atomic_thread_fence(std::memory_order_acquire);
        delete this->data;

        // every SharedPtr holds a single weak ref collectively
        // this is the last SharedPtr so drop the ref
        if (this->control_block->weak.fetch_sub(1, std::memory_order_release) != 1) {
            return;
        }

        // Same reordering-prevention logic here
        std::atomic_thread_fence(std::memory_order_acquire);
        delete this->control_block;
    }

    template<typename U>
    void assign_owner(EnableSharedFromThis<U> const *obj) {
        obj->set_owner(WeakPtr<U>{*this});
    }

    void assign_owner(void const *) {
        // noop for types which are not EnabledSharedFromThis
    }

    explicit SharedPtr(control_block_type *control_block, element_type *data) noexcept : control_block{control_block},
                                                                                         data{data} {
    }
public:
    SharedPtr() noexcept = default;
    SharedPtr(std::nullptr_t) noexcept {}
    SharedPtr(SharedPtr const &other) : control_block{other.control_block},
                                        data{other.data} {
        if (this->control_block == nullptr) {
            return;
        }

        this->control_block->strong.fetch_add(1, std::memory_order_relaxed);
    }

    SharedPtr(SharedPtr &&other) noexcept : control_block{std::exchange(other.control_block, nullptr)},
                                            data{other.data} {
    }

    SharedPtr &operator=(SharedPtr const &other) noexcept {
        if (this == &other || this->control_block == other.control_block) {
            return *this;
        }

        this->drop();
        this->control_block = other.control_block;
        this->data = other.data;

        // using relaxed here is fine as new references can only be formed
        // when there is currently another SharedPtr alive
        //
        // www.boost.org/doc/libs/1_55_0/doc/html/atomic/usage_examples.html
        this->control_block->strong.fetch_add(1, std::memory_order_relaxed);

        return *this;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        this->drop();
        this->control_block = std::exchange(other.control_block, nullptr);
        this->data = other.data;

        return *this;
    }

    ~SharedPtr() noexcept(std::is_nothrow_destructible_v<T>) {
        this->drop();
    }

    /**
     * Creates a shared pointer from a raw pointer.
     * @warning This function takes ownership of the given pointer.
     *
     * @param raw the raw pointer to take ownership of
     * @return a SharedPtr to the provided raw pointer
     * @safety Using the provided pointer in any way after passing it to
     *      this function is undefined behaviour. Especially deleting it manually.
     */
    static SharedPtr from_raw(T *&&raw) noexcept {
        auto *control_block = new control_block_type{
                .strong = 1,
                .weak = 1};

        SharedPtr self{control_block, raw};
        self.assign_owner(raw);

        return self;
    }

    /**
     * equivalent to std::make_shared<T>
     *
     * @param args constructor arguments to construct a T
     * @return a SharedPtr to the constructed T
     */
    template<typename ...Us>
    static SharedPtr make(Us &&...args) noexcept(std::is_nothrow_constructible_v<T, decltype(std::forward<Us>(args))...>) {
        auto *control_block = new control_block_type{
                .strong = 1,
                .weak = 1};

        auto *data = new T{std::forward<Us>(args)...};
        SharedPtr self{control_block, data};
        self.assign_owner(data);

        return self;
    }

    element_type *get() const noexcept {
        assert(this->control_block != nullptr);
        return this->data;
    }

    element_type *operator->() const noexcept {
        return this->get();
    }

    element_type &operator*() const noexcept {
        return *this->get();
    }

    template<typename U> requires std::derived_from<T, U>
    operator SharedPtr<U>() const noexcept {
        if (this->control_block == nullptr) {
            return SharedPtr<U>{};
        }

        // same reasoning for relaxed as in copy-ctor
        this->control_block->strong.fetch_add(1, std::memory_order_relaxed);
        return SharedPtr<U>{this->control_block, static_cast<U *>(this->data)};
    }

    std::strong_ordering operator<=>(SharedPtr const &other) const noexcept {
        return this->control_block <=> other.control_block;
    }
    bool operator==(SharedPtr const &other) const noexcept {
        return this->control_block == other.control_block;
    }
    bool operator!=(SharedPtr const &other) const noexcept {
        return this->control_block != other.control_block;
    }
    bool operator<(SharedPtr const &other) const noexcept {
        return this->control_block < other.control_block;
    }
    bool operator<=(SharedPtr const &other) const noexcept {
        return this->control_block <= other.control_block;
    }
    bool operator>(SharedPtr const &other) const noexcept {
        return this->control_block > other.control_block;
    }
    bool operator>=(SharedPtr const &other) const noexcept {
        return this->control_block >= other.control_block;
    }

    /**
     * @return if this is null
     */
    [[nodiscard]] bool null() const noexcept {
        return this->control_block == nullptr || this->data == nullptr;
    }

    /**
     * @return hash of the control block pointer
     */
    [[nodiscard]] bool ptr_hash() const noexcept {
        return storage::util::robin_hood::hash<control_block_type *>{}(this->control_block);
    }
};

/**
 * This is to SharedPtr what std::weak_ptr is to std::shared_ptr.
 * @tparam T type of object to manage
 */
template<typename T>
struct WeakPtr {
    using strong_type = SharedPtr<T>;
    using element_type = T;

private:
    template<typename U>
    friend struct WeakPtr;

    using control_block_type = shared_ptr_detail::ControlBlock;

    control_block_type *control_block = nullptr;
    element_type *data = nullptr;

    explicit WeakPtr(control_block_type *control_block, element_type *data) : control_block{control_block},
                                                                              data{data} {}

    void drop() noexcept {
        if (this->control_block == nullptr) {
            return;
        }

        if (this->control_block->weak.fetch_sub(1, std::memory_order_release) != 1) {
            return;
        }

        std::atomic_thread_fence(std::memory_order_acquire);
        delete this->control_block;
    }
public:
    WeakPtr() noexcept = default;
    WeakPtr(std::nullptr_t) noexcept {}

    template<typename U> requires std::derived_from<U, T>
    WeakPtr(SharedPtr<U> const &strong) noexcept {
        if (strong.control_block == nullptr) {
            return;
        }

        auto cur = strong.control_block->weak.load(std::memory_order_relaxed);
        while (true) {
            if (strong.control_block->weak.compare_exchange_weak(cur, cur + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
                this->control_block = strong.control_block;
                this->data = strong.data;
                return;
            }
        }
    }

    WeakPtr(WeakPtr const &other) noexcept : control_block{other.control_block},
                                             data{other.data} {
        if (this->control_block == nullptr) {
            return;
        }

        // same reasoning for relaxed as in SharedPtrs copy-ctor
        this->control_block->weak.fetch_add(1, std::memory_order_relaxed);
    }

    WeakPtr(WeakPtr &&other) noexcept : control_block{std::exchange(other.control_block, nullptr)},
                                        data{other.data} {
    }

    WeakPtr &operator=(WeakPtr const &other) noexcept {
        if (this == &other || this->control_block == other.control_block) {
            return *this;
        }

        this->drop();
        this->control_block = other.control_block;
        this->data = other.data;

        // same reasoning for relaxed as in copy-ctor
        this->control_block->weak.fetch_add(1, std::memory_order_relaxed);
        return *this;
    }

    WeakPtr &operator=(WeakPtr &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        this->drop();
        this->control_block = std::exchange(other.control_block, nullptr);
        this->data = other.data;
        return *this;
    }

    ~WeakPtr() {
        this->drop();
    }

    std::optional<strong_type> try_upgrade() const noexcept {
        if (this->control_block == nullptr) {
            // return back nullptr if this is nullptr
            return strong_type{nullptr, nullptr};
        }

        // relaxed is fine here as the value is checked by the following compare exchange loop
        auto cur = this->control_block->strong.load(std::memory_order_relaxed);
        while (true) {
            if (cur == 0) {
                // we can never increase the reference count from 0
                // as this would mean we are giving out a reference to an already destroyed object
                return std::nullopt;
            }

            // relaxed for failure is fine because we don't care about the state in the failure case
            // on success however we need to synchronize with the destructor, therefore need an acquire-load
            if (this->control_block->strong.compare_exchange_weak(cur, cur + 1, std::memory_order_acquire, std::memory_order_relaxed)) {
                return strong_type{this->control_block, this->data};
            }
        }
    }

    strong_type upgrade() const {
        if (auto strong = this->try_upgrade(); strong.has_value()) {
            return *strong;
        }

        throw std::runtime_error{""};
    }

    template<typename U> requires std::derived_from<T, U>
    operator WeakPtr<U>() const noexcept {
        if (this->control_block == nullptr) {
            return WeakPtr<U>{};
        }

        // same reasoning for relaxed as in copy-ctor
        this->control_block->weak.fetch_add(1, std::memory_order_relaxed);
        return WeakPtr<U>{this->control_block, static_cast<U *>(this->data)};
    }

    /**
     * Pointer equality on the _control block_. The comparision
     * is done one the control block and not on the data because the control
     * block is required to be alive when a weak is alive.
     *
     * @param other weak to compare against
     * @return this' control block ptr <=> other's control block ptr
     */
    std::strong_ordering operator<=>(WeakPtr const &other) const noexcept {
        return this->control_block <=> other.control_block;
    }
    bool operator==(WeakPtr const &other) const noexcept {
        return this->control_block == other.control_block;
    }
    bool operator!=(WeakPtr const &other) const noexcept {
        return this->control_block != other.control_block;
    }
    bool operator<(WeakPtr const &other) const noexcept {
        return this->control_block < other.control_block;
    }
    bool operator<=(WeakPtr const &other) const noexcept {
        return this->control_block <= other.control_block;
    }
    bool operator>(WeakPtr const &other) const noexcept {
        return this->control_block > other.control_block;
    }
    bool operator>=(WeakPtr const &other) const noexcept {
        return this->control_block >= other.control_block;
    }

    /**
     * Pointer equality on the _control block_. The comparision
     * is done one the control block and not on the data because the control
     * block is required to be alive when a weak is alive.
     *
     * @param other weak to compare against
     * @return this' control block ptr <=> other's control block ptr
     */
    friend std::strong_ordering operator<=>(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block <=> rhs.control_block;
    }
    friend bool operator==(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block == rhs.control_block;
    }
    friend bool operator!=(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block != rhs.control_block;
    }
    friend bool operator<(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block < rhs.control_block;
    }
    friend bool operator<=(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block <= rhs.control_block;
    }
    friend bool operator>(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block > rhs.control_block;
    }
    friend bool operator>=(strong_type const &lhs, WeakPtr const &rhs) noexcept {
        return lhs.control_block >= rhs.control_block;
    }

    /**
     * Pointer equality on the _control block_. The comparision
     * is done one the control block and not on the data because the control
     * block is required to be alive when a weak is alive.
     *
     * @param other weak to compare against
     * @return this' control block ptr <=> other's control block ptr
     */
    friend std::strong_ordering operator<=>(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block <=> rhs.control_block;
    }
    friend bool operator==(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block == rhs.control_block;
    }
    friend bool operator!=(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block != rhs.control_block;
    }
    friend bool operator<(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block < rhs.control_block;
    }
    friend bool operator<=(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block <= rhs.control_block;
    }
    friend bool operator>(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block > rhs.control_block;
    }
    friend bool operator>=(WeakPtr const &lhs, strong_type const &rhs) noexcept {
        return lhs.control_block >= rhs.control_block;
    }

    /**
     * @return if this is null
     */
    [[nodiscard]] bool null() const noexcept {
        return this->control_block == nullptr || this->data == nullptr;
    }

    /**
     * @return hash of the control block pointer
     */
    [[nodiscard]] bool ptr_hash() const noexcept {
        return storage::util::robin_hood::hash<control_block_type *>{}(this->control_block);
    }
};



}  //namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_RDF_UTIL_SHAREDPTR_HPP
