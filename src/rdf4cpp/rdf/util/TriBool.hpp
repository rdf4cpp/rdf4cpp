#ifndef RDF4CPP_UTIL_TRIBOOL_HPP
#define RDF4CPP_UTIL_TRIBOOL_HPP

#include <array>

namespace rdf4cpp::rdf::util {

/**
 * @brief Represents a boolean with three states
 * the states are: Err (= the error/undefined state), False, True
 */
struct TriBool {
private:
    enum struct Value : int8_t {
        Err = -1,
        False = 0,
        True = 1
    };

    Value value;

    [[nodiscard]] constexpr size_t into_logic_table_index() const noexcept {
        return static_cast<std::underlying_type_t<Value>>(this->value) + 1;
    }

    constexpr TriBool(Value const value) : value{value} {}
public:
    static TriBool const True;
    static TriBool const False;
    static TriBool const Err;

    /**
     * @brief implicit conversion from bool; maps true -> TriBool::True and false -> TriBool::False
     */
    constexpr TriBool(bool const b) : value{ static_cast<std::underlying_type_t<Value>>(b) } {}

    constexpr bool operator==(TriBool const &) const noexcept = default;
    constexpr bool operator!=(TriBool const &) const noexcept = default;

    /**
     * @brief implicit conversion to bool
     * @return true if *this == TriBool::True; otherwise false
     */
    constexpr operator bool() const noexcept {
        return this->value == Value::True;
    }

    friend constexpr TriBool operator!(TriBool const b) noexcept {
        constexpr std::array<Value, 3> not_logic_table{
             /* Err         False        True */
                Value::Err, Value::True, Value::False};

        return not_logic_table[b.into_logic_table_index()];
    }

    friend constexpr TriBool operator&&(TriBool const l, TriBool const r) noexcept {
        constexpr std::array<std::array<Value, 3>, 3> and_logic_table{
                /* lhs \ rhs               Err           False         True */
                /* Err       */ std::array{Value::Err,   Value::False, Value::Err},
                /* False     */ std::array{Value::False, Value::False, Value::False},
                /* True      */ std::array{Value::Err,   Value::False, Value::True}};

        return and_logic_table[l.into_logic_table_index()][r.into_logic_table_index()];
    }

    friend constexpr TriBool operator&&(TriBool const l, bool const r) noexcept {
        return l && TriBool{r};
    }

    friend constexpr TriBool operator&&(bool const l, TriBool const r) noexcept {
        return TriBool{l} && r;
    }

    friend constexpr TriBool operator||(TriBool const l, TriBool const r) noexcept {
        constexpr std::array<std::array<Value, 3>, 3> or_logic_table{
                /* lhs \ rhs               Err          False         True */
                /* Err       */ std::array{Value::Err,  Value::Err,   Value::True},
                /* False     */ std::array{Value::Err,  Value::False, Value::True},
                /* True      */ std::array{Value::True, Value::True,  Value::True}};

        return or_logic_table[l.into_logic_table_index()][r.into_logic_table_index()];
    }

    friend constexpr TriBool operator||(TriBool const l, bool const r) noexcept {
        return l || TriBool{r};
    }

    friend constexpr TriBool operator||(bool const l, TriBool const r) noexcept {
        return TriBool{l} || r;
    }
};

inline constexpr TriBool TriBool::True{TriBool::Value::True};
inline constexpr TriBool TriBool::False{TriBool::Value::False};
inline constexpr TriBool TriBool::Err{TriBool::Value::Err};

/**
 * @brief Checks whether the value of the partial_ordering (l) is equal to the value of the weak ordering (r)
 * @return TriBool::Err if l is unordered; otherwise return l == r
 */
constexpr TriBool partial_ordering_eq(std::partial_ordering const l, std::weak_ordering const r) {
    if (l == std::partial_ordering::unordered) {
        return TriBool::Err;
    }

    return l == r;
}

} // namespace rdf4cpp::rdf::util

#endif // RDF4CPP_UTIL_TRIBOOL_HPP
