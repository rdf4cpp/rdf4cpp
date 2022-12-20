#ifndef RDF4CPP_UTIL_COWSTRING_HPP
#define RDF4CPP_UTIL_COWSTRING_HPP

#include <string>
#include <string_view>
#include <variant>

namespace rdf4cpp::rdf::util {

/**
 * Tags to control the construction of CowStrings
 */
namespace ownership_tag {

struct Owned {};
struct Borrowed {};

inline constexpr Borrowed borrowed;
inline constexpr Owned owned;

} // namespace ownership_tags

/**
 * A string-like type, that is either std::string_view (borrowed) or std::string (owned) with copy-on-write functionality.
 * Inspired by rust's std::borrow::Cow (https://doc.rust-lang.org/std/borrow/enum.Cow.html).
 */
struct CowString {
private:
    using repr_t = std::variant<std::string, std::string_view>;
    repr_t data;

public:
    constexpr CowString(ownership_tag::Borrowed, std::string_view const value) noexcept : data{std::in_place_type<std::string_view>, value} {}

    constexpr CowString(ownership_tag::Owned, std::string const &value) : data{std::in_place_type<std::string>, value} {}
    constexpr CowString(ownership_tag::Owned, std::string &&value) noexcept : data{std::in_place_type<std::string>, std::move(value)} {}

    inline operator std::string_view() const noexcept {
        return this->view();
    }

    [[nodiscard]] constexpr std::string_view view() const noexcept {
        return std::visit([](auto const &value) noexcept -> std::string_view {
            return value;
        }, this->data);
    }

    [[nodiscard]] constexpr bool is_borrowed() const noexcept {
        return this->data.index() == 1;
    }

    [[nodiscard]] constexpr bool is_owned() const noexcept {
        return this->data.index() == 0;
    }

    [[nodiscard]] constexpr std::string_view get_borrowed() const {
        return std::get<std::string_view>(this->data);
    }

    [[nodiscard]] constexpr std::string const &get_owned() const {
        return std::get<std::string>(this->data);
    }

    [[nodiscard]] constexpr std::string &get_owned() {
        return std::get<std::string>(this->data);
    }

    /**
     * Converts this CowString into std::string by copy
     */
    [[nodiscard]] constexpr std::string into_owned() const & noexcept {
        if (this->is_borrowed()) {
            return std::string{this->get_borrowed()};
        }

        return this->get_owned();
    }

    /**
     * Converts this CowString into std::string (by move if the owned form is active)
     */
    [[nodiscard]] constexpr std::string into_owned() && noexcept {
        if (this->is_borrowed()) {
            return std::string{this->get_borrowed()};
        }

        return std::move(this->get_owned());
    }

    /**
     * Returns a non-const reference to the owned form of the data (std::string)
     * Converts the data to the owned for if it is currently the borrowed form
     */
    [[nodiscard]] constexpr std::string &to_mutable() noexcept {
        if (this->is_borrowed()) {
            this->data = std::string{this->get_borrowed()};
        }

        return this->get_owned();
    }

    constexpr std::strong_ordering operator<=>(CowString const &other) const noexcept {
        return this->view() <=> other.view();
    }

    friend constexpr bool operator==(CowString const &lhs, std::string_view const rhs) noexcept {
        return lhs.view() == rhs;
    }
    friend constexpr bool operator==(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs == rhs.view();
    }

    friend constexpr bool operator!=(CowString const &lhs, std::string_view const rhs) noexcept {
        return lhs.view() != rhs;
    }
    friend constexpr bool operator!=(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs != rhs.view();
    }

    friend constexpr bool operator<(CowString const &lhs, std::string_view const rhs) noexcept {
        return lhs.view() < rhs;
    }
    friend constexpr bool operator<(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs < rhs.view();
    }

    friend constexpr bool operator>(CowString const &lhs, std::string_view const rhs) noexcept {
        return lhs.view() > rhs;
    }
    friend constexpr bool operator>(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs > rhs.view();
    }

    friend constexpr bool operator<=(CowString const &lhs, std::string_view const rhs) noexcept {
        return lhs.view() <= rhs;
    }
    friend constexpr bool operator<=(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs <= rhs.view();
    }

    friend constexpr bool operator>=(CowString const &lhs, std::string_view const rhs) noexcept {
        return lhs.view() >= rhs;
    }
    friend constexpr bool operator>=(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs >= rhs.view();
    }

    friend inline std::ostream &operator<<(std::ostream &os, CowString const &cow) {
        os << cow.view();
        return os;
    }
};

} // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_UTIL_COWSTRING_HPP
