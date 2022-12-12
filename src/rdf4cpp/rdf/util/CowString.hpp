#ifndef RDF4CPP_UTIL_COWSTRING_HPP
#define RDF4CPP_UTIL_COWSTRING_HPP

#include <string>
#include <string_view>
#include <variant>

namespace rdf4cpp::rdf::util {

struct CowString {
private:
    using repr_t = std::variant<std::string, std::string_view>;
    repr_t data;

public:
    CowString(std::string const &value) : data{value} {}
    CowString(std::string &&value) : data{std::move(value)} {}
    CowString(std::string_view const value) : data{value} {}

    inline operator std::string_view() const noexcept {
        return std::visit([](auto const &value) noexcept -> std::string_view {
            return value;
        }, this->data);
    }

    inline operator std::string() const noexcept {
        return std::string{*this};
    }

    inline std::strong_ordering operator<=>(CowString const &other) const noexcept {
        return static_cast<std::string_view>(*this) <=> static_cast<std::string_view>(other);
    }

    inline friend std::strong_ordering operator<=>(CowString const &lhs, char const *rhs) noexcept {
        return static_cast<std::string_view>(lhs) <=> rhs;
    }

    inline friend std::strong_ordering operator<=>(char const *lhs, CowString const &rhs) noexcept {
        return lhs <=> static_cast<std::string_view>(rhs);
    }

    inline friend std::strong_ordering operator<=>(CowString const &lhs, std::string_view const rhs) noexcept {
        return static_cast<std::string_view>(lhs) <=> rhs;
    }

    inline friend std::strong_ordering operator<=>(std::string_view const lhs, CowString const &rhs) noexcept {
        return lhs <=> static_cast<std::string_view>(rhs);
    }

    template<size_t N>
    friend std::strong_ordering operator<=>(CowString const &lhs, char const (&rhs)[N]) noexcept {
        return static_cast<std::string_view>(lhs) <=> rhs;
    }

    template<size_t N>
    friend std::strong_ordering operator<=>(char const (&lhs)[N], CowString const &rhs) noexcept {
        return lhs <=> static_cast<std::string_view>(rhs);
    }
};

} // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_UTIL_COWSTRING_HPP
