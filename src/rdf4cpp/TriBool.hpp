#ifndef RDF4CPP_UTIL_TRIBOOL_HPP
#define RDF4CPP_UTIL_TRIBOOL_HPP

#include <array>
#include <cstddef>
#include <cstdint>

namespace rdf4cpp {

/**
 * @brief Represents a boolean with three states
 * the states are: Err (= the error/undefined state), False, True
 */
struct TriBool {
    static TriBool const True;
    static TriBool const False;
    static TriBool const Err;

private:
    enum struct inner_value_type : int8_t {
        Err = -1,
        False = 0,
        True = 1
    };

    inner_value_type value;

    [[nodiscard]] constexpr size_t into_logic_table_index() const noexcept {
        return static_cast<std::underlying_type_t<inner_value_type>>(this->value) + 1;
    }

    static constexpr std::array<std::array<inner_value_type, 3>, 3> and_logic_table{
            /* lhs \ rhs               Err                      False                    True */
            /* Err       */ std::array{inner_value_type::Err,   inner_value_type::False, inner_value_type::Err},
            /* False     */ std::array{inner_value_type::False, inner_value_type::False, inner_value_type::False},
            /* True      */ std::array{inner_value_type::Err,   inner_value_type::False, inner_value_type::True}};

    static constexpr std::array<std::array<inner_value_type, 3>, 3> or_logic_table{
            /* lhs \ rhs               Err                     False                    True */
            /* Err       */ std::array{inner_value_type::Err,  inner_value_type::Err,   inner_value_type::True},
            /* False     */ std::array{inner_value_type::Err,  inner_value_type::False, inner_value_type::True},
            /* True      */ std::array{inner_value_type::True, inner_value_type::True,  inner_value_type::True}};

    static constexpr std::array<inner_value_type, 3> not_logic_table{
            /* Err         False        True */
            inner_value_type::Err, inner_value_type::True, inner_value_type::False};

    constexpr TriBool(inner_value_type const value) noexcept : value{value} {}

public:
    /**
     * @brief implicit conversion from bool; maps true -> TriBool::True and false -> TriBool::False
     */
    constexpr TriBool(bool const b) noexcept : value{static_cast<std::underlying_type_t<inner_value_type>>(b)} {}

    constexpr bool operator==(TriBool const &) const noexcept = default;
    constexpr bool operator!=(TriBool const &) const noexcept = default;

    /**
     * @brief implicit conversion to bool
     * @return true if *this == TriBool::True; otherwise false
     */
    constexpr operator bool() const noexcept {
        return this->value == inner_value_type::True;
    }

    friend constexpr TriBool operator!(TriBool const b) noexcept {
        return not_logic_table[b.into_logic_table_index()];
    }

    friend constexpr TriBool operator&&(TriBool const l, TriBool const r) noexcept {
        return and_logic_table[l.into_logic_table_index()][r.into_logic_table_index()];
    }

    friend constexpr TriBool operator&&(TriBool const l, bool const r) noexcept {
        return l && TriBool{r};
    }

    friend constexpr TriBool operator&&(bool const l, TriBool const r) noexcept {
        return TriBool{l} && r;
    }

    friend constexpr TriBool operator||(TriBool const l, TriBool const r) noexcept {
        return or_logic_table[l.into_logic_table_index()][r.into_logic_table_index()];
    }

    friend constexpr TriBool operator||(TriBool const l, bool const r) noexcept {
        return l || TriBool{r};
    }

    friend constexpr TriBool operator||(bool const l, TriBool const r) noexcept {
        return TriBool{l} || r;
    }
};

inline constexpr TriBool TriBool::True{TriBool::inner_value_type::True};
inline constexpr TriBool TriBool::False{TriBool::inner_value_type::False};
inline constexpr TriBool TriBool::Err{TriBool::inner_value_type::Err};

namespace util {

/**
 * @brief Checks whether the value of the partial_ordering (l) is equal to the value of the weak ordering (r)
 * @return TriBool::Err if l is unordered; otherwise return l == r
 */
constexpr TriBool partial_weak_ordering_eq(std::partial_ordering const l, std::weak_ordering const r) noexcept {
    if (l == std::partial_ordering::unordered) {
        return TriBool::Err;
    }

    return l == r;
}

} // namespace util

} // namespace rdf4cpp

#endif // RDF4CPP_UTIL_TRIBOOL_HPP
