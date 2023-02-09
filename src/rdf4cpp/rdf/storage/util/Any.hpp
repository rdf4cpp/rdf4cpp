#ifndef RDF4CPP_ANY_HPP
#define RDF4CPP_ANY_HPP

#include <concepts>
#include <cstddef>
#include <utility>
#include <typeinfo>
#include <type_traits>

namespace rdf4cpp::rdf::storage::util {

namespace any_detail {

union AnyStorage {
    alignas(void *) std::byte small[sizeof(void *)];
    void *big;
};

struct ManagerBase {
    virtual void clone(AnyStorage &dst, AnyStorage const &src) const noexcept = 0;
    virtual void move(AnyStorage &dst, AnyStorage &&src) const noexcept = 0;
    virtual void destroy(AnyStorage &self) const = 0;

    [[nodiscard]] virtual void *get_mut(AnyStorage &self) const noexcept = 0;
    [[nodiscard]] virtual void const *get(AnyStorage const &self) const noexcept = 0;

    [[nodiscard]] virtual size_t hash(AnyStorage const &self) const noexcept = 0;
    [[nodiscard]] virtual bool eq(AnyStorage const &lhs, AnyStorage const &rhs) const noexcept = 0;
};

template<typename T>
struct ExternalManager final : ManagerBase {
    constexpr ExternalManager() noexcept = default;

    inline void clone(AnyStorage &dst, AnyStorage const &src) const noexcept override {
        dst.big = new T{*reinterpret_cast<T const *>(src.big)};
    }

    inline void move(AnyStorage &dst, AnyStorage &&src) const noexcept override {
        dst.big = std::exchange(src.big, nullptr);
    }

    inline void destroy(AnyStorage &self) const override {
        delete reinterpret_cast<T *>(self.big);
    }

    [[nodiscard]] inline void *get_mut(AnyStorage &self) const noexcept override {
        return self.big;
    }

    [[nodiscard]] inline void const *get(AnyStorage const &self) const noexcept override {
        return self.big;
    }

    [[nodiscard]] inline size_t hash(AnyStorage const &self) const noexcept override {
        return std::hash<T>{}(*reinterpret_cast<T const *>(self.big));
    }

    [[nodiscard]] inline bool eq(AnyStorage const &lhs, AnyStorage const &rhs) const noexcept override {
        return reinterpret_cast<T const *>(lhs.big) == reinterpret_cast<T const *>(rhs.big);
    }
};

template<typename T>
struct InternalManager final : ManagerBase {
    constexpr InternalManager() noexcept = default;

    inline void clone(AnyStorage &dst, AnyStorage const &src) const noexcept override {
        new (&dst.small) T{*reinterpret_cast<T const *>(&src.small)};
    }

    inline void move(AnyStorage &dst, AnyStorage &&src) const noexcept override {
        new (&dst.small) T{std::move(*reinterpret_cast<T *>(&src.small))};
    }

    inline void destroy(AnyStorage &self) const override {
        reinterpret_cast<T *>(&self.small)->~T();
    }

    [[nodiscard]] inline void *get_mut(AnyStorage &self) const noexcept override {
        return &self.small;
    }

    [[nodiscard]] inline void const *get(AnyStorage const &self) const noexcept override {
        return &self.small;
    }

    [[nodiscard]] inline size_t hash(AnyStorage const &self) const noexcept override {
        return std::hash<T>{}(*reinterpret_cast<T const *>(&self.small));
    }

    [[nodiscard]] inline bool eq(AnyStorage const &lhs, AnyStorage const &rhs) const noexcept override {
        return *reinterpret_cast<T const *>(&lhs.small) == *reinterpret_cast<T const *>(&rhs.small);
    }
};

}  //namespace any_detail


template<typename T>
concept AnyHoldable = std::is_copy_constructible_v<T> &&
                      std::equality_comparable<T> &&
                      requires (T const &a, T const &b) {
                          { std::hash<T>{}(a) } -> std::convertible_to<size_t>;
                      };

struct Any {
private:
    std::type_info const *tinfo;
    any_detail::ManagerBase const *manager;
    any_detail::AnyStorage storage;

    template<typename T>
    static constexpr bool fits = sizeof(T) <= sizeof(any_detail::AnyStorage) &&
                                 alignof(T) <= alignof(any_detail::AnyStorage) &&
                                 std::is_nothrow_move_constructible_v<T>;

    template<typename T>
    static constexpr std::conditional_t<fits<T>,
                                        any_detail::InternalManager<T>,
                                        any_detail::ExternalManager<T>> static_manager{};

    template<typename T, typename DecayT = std::decay_t<T>>
    using decay_if_not_any = std::enable_if_t<!std::is_same_v<DecayT, Any>, DecayT>;

    inline void drop() {
        if (this->manager != nullptr) {
            manager->destroy(this->storage);
        }
    }
public:
    template<typename T, typename DecayT = decay_if_not_any<T>> requires (AnyHoldable<std::decay_t<T>>)
    explicit Any(T &&value) noexcept(std::is_nothrow_constructible_v<DecayT, decltype(std::forward<T>(value))>)
        : tinfo{&typeid(DecayT)},
          manager{&static_manager<DecayT>} {

        if constexpr (fits<DecayT>) {
            new (&this->storage.small) DecayT{std::forward<T>(value)};
        } else {
            this->storage.big = new DecayT{std::forward<T>(value)};
        }
    }

    template<typename T, typename ...Args> requires AnyHoldable<T>
    explicit Any(std::in_place_type_t<T>, Args &&...args) noexcept(std::is_nothrow_constructible_v<T, decltype(std::forward<Args>(args))...>)
        : tinfo{&typeid(T)},
          manager{&static_manager<T>} {

        if constexpr (fits<T>) {
            new (&this->storage.small) T{std::forward<Args>(args)...};
        } else {
            this->storage.big = new T{std::forward<Args>(args)...};
        }
    }

    inline Any(Any const &other) : tinfo{other.tinfo},
                                   manager{other.manager} {
        this->manager->clone(this->storage, other.storage);
    }

    inline Any(Any &&other) noexcept : tinfo{other.tinfo},
                                       manager{std::exchange(other.manager, nullptr)} {
        this->manager->move(this->storage, std::move(other.storage));
    }

    inline Any &operator=(Any const &other) {
        if (this == &other) {
            return *this;
        }

        this->drop();
        this->manager = other.manager;
        this->tinfo = other.tinfo;
        this->manager->clone(this->storage, other.storage);
        return *this;
    }

    inline Any &operator=(Any &&other) noexcept {
        if (this == &other) {
            return *this;
        }

        this->drop();
        this->manager = std::exchange(other.manager, nullptr);
        this->tinfo = other.tinfo;
        this->manager->move(this->storage, std::move(other.storage));
        return *this;
    }

    inline ~Any() {
        this->drop();
    }

    template<typename T>
    T const &get_unchecked() const noexcept {
        assert(*this->tinfo == typeid(T));
        return *reinterpret_cast<T const *>(this->manager->get(this->storage));
    }

    template<typename T>
    T &get_unchecked() noexcept {
        assert(*this->tinfo == typeid(T));
        return *reinterpret_cast<T *>(this->manager->get_mut(this->storage));
    }

    template<typename T>
    T const &get() const {
        if (*this->tinfo != typeid(T)) {
            throw std::runtime_error{"bad any cast"};
        }

        return this->get_unchecked<T>();
    }

    template<typename T>
    T &get() {
        if (*this->tinfo != typeid(T)) {
            throw std::runtime_error{"bad any cast"};
        }

        return this->get_unchecked<T>();
    }

    [[nodiscard]] inline size_t hash() const noexcept {
        return this->manager->hash(this->storage);
    }

    inline bool operator==(Any const &other) const noexcept {
        if (*this->tinfo != *other.tinfo) {
            return false;
        }

        return this->manager->eq(this->storage, other.storage);
    }

    inline bool operator!=(Any const &other) const noexcept {
        return !(*this == other);
    }
};

}  //namespace rdf4cpp::rdf::util

template<>
struct std::hash<rdf4cpp::rdf::storage::util::Any> {
    inline size_t operator()(rdf4cpp::rdf::storage::util::Any const &self) const noexcept {
        return self.hash();
    }
};

#endif  //RDF4CPP_ANY_HPP
