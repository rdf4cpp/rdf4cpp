#ifndef RDF4CPP_UTIL_COWSTRING_HPP
#define RDF4CPP_UTIL_COWSTRING_HPP

#include <string>
#include <string_view>
#include <variant>

namespace rdf4cpp {

/**
 * A string-like type, that is either std::string_view (borrowed) or std::string (owned) with copy-on-write functionality.
 * Inspired by rust's std::borrow::Cow (https://doc.rust-lang.org/std/borrow/enum.Cow.html).
 */
struct CowString {
    using value_type = std::string_view::value_type;
    using traits_type = std::string_view::traits_type;
    using iterator = std::string_view::iterator;
    using const_iterator = std::string_view::const_iterator;
    using const_reverse_iterator = std::string_view::const_reverse_iterator;
    using reverse_iterator = std::string_view::reverse_iterator;
    using size_type = std::string_view::size_type;

    struct owned_tag {};
    struct borrowed_tag {};

    static constexpr owned_tag owned{};
    static constexpr borrowed_tag borrowed{};

private:
    using repr_t = std::variant<std::string, std::string_view>;
    repr_t data_;

public:
    constexpr CowString(borrowed_tag, std::string_view const value) noexcept : data_{std::in_place_type<std::string_view>, value} {}

    CowString(owned_tag, std::string const &value) : data_{std::in_place_type<std::string>, value} {}
    CowString(owned_tag, std::string &&value) noexcept : data_{std::in_place_type<std::string>, std::move(value)} {}

    [[nodiscard]] constexpr size_type size() const noexcept {
        return this->view().size();
    }

    [[nodiscard]] constexpr value_type const *data() const noexcept {
        return this->view().data();
    }

    [[nodiscard]] constexpr bool empty() const noexcept {
        return this->view().empty();
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept {
        return this->view().begin();
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept {
        return this->view().end();
    }

    [[nodiscard]] constexpr const_iterator cbegin() const noexcept {
        return this->begin();
    }

    [[nodiscard]] constexpr const_iterator cend() const noexcept  {
        return this->end();
    }

    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
        return this->view().rbegin();
    }

    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
        return this->view().rend();
    }

    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
        return this->rbegin();
    }

    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept  {
        return this->rend();
    }

    constexpr operator std::string_view() const noexcept {
        return this->view();
    }

    [[nodiscard]] constexpr std::string_view view() const noexcept {
        return std::visit([](auto const &value) noexcept -> std::string_view {
            return value;
        }, this->data_);
    }

    [[nodiscard]] constexpr bool is_borrowed() const noexcept {
        return this->data_.index() == 1;
    }

    [[nodiscard]] constexpr bool is_owned() const noexcept {
        return this->data_.index() == 0;
    }

    [[nodiscard]] constexpr std::string_view get_borrowed() const {
        return std::get<std::string_view>(this->data_);
    }

    [[nodiscard]] constexpr std::string const &get_owned() const {
        return std::get<std::string>(this->data_);
    }

    [[nodiscard]] constexpr std::string &get_owned() {
        return std::get<std::string>(this->data_);
    }

    /**
     * Converts this CowString into std::string by copy
     */
    [[nodiscard]] std::string into_owned() const & noexcept {
        if (this->is_borrowed()) {
            return std::string{this->get_borrowed()};
        }

        return this->get_owned();
    }

    /**
     * Converts this CowString into std::string (by move if the owned form is active)
     */
    [[nodiscard]] std::string into_owned() && noexcept {
        if (this->is_borrowed()) {
            return std::string{this->get_borrowed()};
        }

        return std::move(this->get_owned());
    }

    /**
     * Returns a non-const reference to the owned form of the data (std::string)
     * Converts the data to the owned for if it is currently the borrowed form
     */
    [[nodiscard]] std::string &to_mutable() noexcept {
        if (this->is_borrowed()) {
            this->data_ = std::string{this->get_borrowed()};
        }

        return this->get_owned();
    }

    constexpr std::strong_ordering operator<=>(CowString const &other) const noexcept {
        return this->view() <=> other.view();
    }

    constexpr bool operator==(CowString const &other) const noexcept {
        return this->view() == other.view();
    }

    constexpr std::strong_ordering operator<=>(std::string_view const rhs) const noexcept {
        return this->view() <=> rhs;
    }

    constexpr bool operator==(std::string_view const rhs) const noexcept {
        return this->view() == rhs;
    }

    friend std::ostream &operator<<(std::ostream &os, CowString const &cow) {
        os << cow.view();
        return os;
    }
};

} // namespace rdf4cpp

#endif  //RDF4CPP_UTIL_COWSTRING_HPP
