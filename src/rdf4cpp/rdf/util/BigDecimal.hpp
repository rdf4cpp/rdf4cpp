#ifndef RDF4CPP_BIGDECIMAL_H
#define RDF4CPP_BIGDECIMAL_H

#include <functional>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>
#include <nonstd/expected.hpp>

namespace rdf4cpp::rdf::util {
enum class RoundingMode {
    Floor,
    Ceil,
    Round,
};

enum class Sign {
    Positive = 0,
    Negative = 1,
};
[[nodiscard]] constexpr Sign operator^(Sign a, Sign b) noexcept {
    using T = std::underlying_type_t<Sign>;
    return static_cast<Sign>(static_cast<T>(static_cast<T>(a) ^ static_cast<T>(b)));
}
[[nodiscard]] constexpr Sign operator!(Sign a) noexcept {
    using T = std::underlying_type_t<Sign>;
    return static_cast<Sign>(static_cast<T>(!static_cast<T>(a)));
}

enum class DecimalError {
    Overflow,
    Underflow,
    NotDefined,  // aka NotANumber
};

template<typename T>
concept BigDecimalBaseType = (std::unsigned_integral<T> && sizeof(T) >= sizeof(int32_t)) || std::same_as<T, boost::multiprecision::cpp_int>;

template<BigDecimalBaseType UnscaledValue_t = boost::multiprecision::cpp_int, BigDecimalBaseType Exponent_t = uint32_t>
class BigDecimal {
    UnscaledValue_t unscaled_value;
    Exponent_t exponent;
    Sign sign = Sign::Positive;

    static constexpr uint32_t base = 10;

    enum class OverflowMode {
        Checked,
        UndefinedBehavior,
    };

    template<OverflowMode m, class T>
    static constexpr bool add_checked(const T &a, const T &b, T &result) noexcept {
        if constexpr (std::is_integral_v<T> && m == OverflowMode::Checked) {
            return __builtin_add_overflow(a, b, &result);
        } else {
            result = a + b;
            return false;
        }
    }

    template<OverflowMode m, class T>
    static constexpr bool sub_checked(const T &a, const T &b, T &result) noexcept {
        if constexpr (std::is_integral_v<T> && m == OverflowMode::Checked) {
            return __builtin_sub_overflow(a, b, &result);
        } else {
            result = a - b;
            return false;
        }
    }

    template<OverflowMode m, class T>
    static constexpr bool mul_checked(const T &a, const T &b, T &result) noexcept {
        if constexpr (std::is_integral_v<T> && m == OverflowMode::Checked) {
            return __builtin_mul_overflow(a, b, &result);
        } else {
            result = a * b;
            return false;
        }
    }

    template<OverflowMode m, class T>
    static constexpr bool pow_checked(const T &a, unsigned int b, T &result) noexcept {
        if constexpr (std::is_integral_v<T>) {
            T r = 1;
            bool over = false;
            for (unsigned int i = 0; i < b; ++i)
                over |= mul_checked<m, T>(r, a, r);
            result = r;
            return over;
        } else {
            result = boost::multiprecision::pow(a, b);
            return false;
        }
    }

    static constexpr UnscaledValue_t abs(const UnscaledValue_t &value) noexcept {
        if constexpr (std::is_integral_v<UnscaledValue_t> && !std::is_signed_v<UnscaledValue_t>) {
            return value;
        } else {
            return value < 0 ? -value : value;
        }
    }

    /**
     * sign logic for ctor
     */
    static constexpr Sign init_sign(const UnscaledValue_t &unscaled_value, Sign s) noexcept {
        if (unscaled_value == 0)
            return Sign::Positive;
        if constexpr (!std::is_integral_v<UnscaledValue_t>) {
            if (unscaled_value < 0)  // only possible with cpp_int
                return !s;
        }
        return s;
    }

public:
    /**
     * creates a BigDecimal from its components.
     * it has the value of sign * unscaled_value * pow(10, -exponent).
     * @param unscaled_value
     * @param exponent
     * @param sign
     */
    constexpr BigDecimal(const UnscaledValue_t &unscaled_value, Exponent_t exponent, Sign sign = Sign::Positive) noexcept
        : unscaled_value(abs(unscaled_value)), exponent(exponent), sign(init_sign(unscaled_value, sign)) {}

    /**
     * parses a BigDecimal from a string_view.
     * may include a leading sign and one decimal point ., everything else needs to be numeric.
     * @param value
     * @throw std::invalid_argument if a invalid char is found
     * @throw std::overflow_error on exceeding the types numeric limits
     */
    constexpr explicit BigDecimal(std::string_view value) : unscaled_value(0), exponent(0) {
        bool begin = true;
        bool decimal = false;
        for (const char c : value) {
            if (begin) {
                begin = false;
                if (c == '-') {
                    sign = Sign::Negative;
                    continue;
                } else if (c == '+')
                    continue;
            }
            if (c == '.') {
                if (decimal)
                    throw std::invalid_argument{"more than one . found"};
                decimal = true;
                continue;
            }
            if (c < '0' || c > '9')
                throw std::invalid_argument{"non-numeric char found"};
            auto n = c - '0';
            if (mul_checked<OverflowMode::Checked>(unscaled_value, UnscaledValue_t{base}, unscaled_value))
                throw std::overflow_error{"ctor unscaled_value overflow"};
            if (add_checked<OverflowMode::Checked>(unscaled_value, UnscaledValue_t{n}, unscaled_value))
                throw std::overflow_error{"ctor unscaled_value overflow"};
            if (decimal) {
                if (add_checked<OverflowMode::Checked>(exponent, Exponent_t{1}, exponent))
                    throw std::overflow_error{"ctor exponent overflow"};
            }
        }
        if (unscaled_value == 0)
            sign = Sign::Positive;
    }

    /**
     * converts a uint32_t to a BigDecimal
     * @param value
     */
    constexpr explicit BigDecimal(uint32_t value) : BigDecimal(UnscaledValue_t{value}, 0) {}

    /**
     * converts a uint64_t to a BigDecimal
     * @param value
     */
    constexpr explicit BigDecimal(uint64_t value) : BigDecimal(UnscaledValue_t{value}, 0) {}

    /**
     * converts a int32_t to a BigDecimal
     * @param value
     */
    constexpr explicit BigDecimal(int32_t value) : BigDecimal(static_cast<UnscaledValue_t>(std::abs(value)), 0, value < 0 ? Sign::Negative : Sign::Positive) {}

    /**
     * converts a int64_t to a BigDecimal
     * @param value
     */
    constexpr explicit BigDecimal(int64_t value) : BigDecimal(static_cast<UnscaledValue_t>(std::abs(value)), 0, value < 0 ? Sign::Negative : Sign::Positive) {}

    /**
     * converts a UnscaledValue_t to a BigDecimal
     * @param value
     */
    constexpr explicit BigDecimal(const UnscaledValue_t &value) : BigDecimal(value, 0) {}

    /**
     * converts a float to a BigDecimal.
     * this conversion might not be exact, due to the built in limitations of floats.
     * if you have the possibility, use one of the other constructors.
     * @param value
     * @throw std::overflow_error on exceeding the types numeric limits
     */
    constexpr explicit BigDecimal(float value) : BigDecimal(static_cast<double>(value)) {}

    /**
     * converts a double to a BigDecimal.
     * this conversion might not be exact, due to the built in limitations of doubles.
     * if you have the possibility, use one of the other constructors.
     * @param value
     * @throw std::overflow_error on exceeding the types numeric limits
     */
    explicit BigDecimal(double value) : unscaled_value(0), exponent(0) {
        if (std::isinf(value) || std::isnan(value))
            throw std::invalid_argument{"value is NaN or infinity"};
        // this might fail on anything that is not x86-32/64
        uint64_t v = *reinterpret_cast<uint64_t *>(&value);
        sign = (v >> 63) == 0 ? Sign::Positive : Sign::Negative;
        auto ex = static_cast<int>((v >> 52) & 0x7ffL);
        uint64_t significand = ex == 0
                                       ? (v & ((1L << 52) - 1)) << 1
                                       : (v & ((1L << 52) - 1)) | (1L << 52);
        ex -= 1075;
        if (significand == 0) {
            sign = Sign::Positive;
            return;
        }
        while (significand & 1) {
            significand >>= 1;
            ++ex;
        }
        static constexpr const char *exc = "double to BigDecimal overflow";
        if (ex == 0) {
            unscaled_value = UnscaledValue_t{significand};
        } else if (ex < 0) {
            exponent = static_cast<Exponent_t>(-ex);
            UnscaledValue_t e{0};
            if (pow_checked<OverflowMode::Checked>(UnscaledValue_t{5}, -ex, e))
                throw std::overflow_error{exc};
            if (mul_checked<OverflowMode::Checked>(UnscaledValue_t{significand}, e, unscaled_value))
                throw std::overflow_error{exc};
        } else {
            UnscaledValue_t e{0};
            if (pow_checked<OverflowMode::Checked>(UnscaledValue_t{2}, ex, e))
                throw std::overflow_error{exc};
            if (mul_checked<OverflowMode::Checked>(UnscaledValue_t{significand}, e, unscaled_value))
                throw std::overflow_error{exc};
        }
        normalize();
    }

    /**
     * converts this BigDecimal to its smallest internal representation.
     */
    constexpr void normalize() noexcept {
        while (exponent > 0 && unscaled_value % base == 0) {
            unscaled_value /= base;
            --exponent;
        }
    }

    [[nodiscard]] constexpr Exponent_t get_exponent() const noexcept {
        return exponent;
    }

private:
    template<OverflowMode m>
    constexpr bool add(const BigDecimal &other, BigDecimal &result) const noexcept {
        UnscaledValue_t t = this->unscaled_value;
        UnscaledValue_t o = other.unscaled_value;
        Exponent_t new_exp = std::max(this->exponent, other.exponent);
        if (this->exponent < new_exp) {
            UnscaledValue_t ex{0};
            if (pow_checked<m>(UnscaledValue_t{base}, new_exp - this->exponent, ex))
                return true;
            if (mul_checked<m>(t, ex, t))
                return true;
        } else if (other.exponent < new_exp) {
            UnscaledValue_t ex{0};
            if (pow_checked<m>(UnscaledValue_t{base}, new_exp - other.exponent, ex))
                return true;
            if (mul_checked<m>(o, ex, o))
                return true;
        }
        UnscaledValue_t res = 0;
        Sign s = Sign::Positive;
        if (this->sign == other.sign) {
            if (add_checked<m>(t, o, res))
                return true;
            s = this->sign;
        } else if (t > o) {
            if (sub_checked<m>(t, o, res))
                return true;
            s = this->sign;
        } else if (t < o) {
            if (sub_checked<m>(o, t, res))
                return true;
            s = other.sign;
        }
        result = BigDecimal{res, new_exp, s};
        return false;
    }

    template<OverflowMode m>
    constexpr bool mul(const BigDecimal &other, BigDecimal &result) const noexcept {
        UnscaledValue_t v{0};
        if (mul_checked<m>(this->unscaled_value, other.unscaled_value, v))
            return true;
        Exponent_t e{0};
        if (add_checked<m>(this->exponent, other.exponent, e))
            return true;
        result = BigDecimal{v, e, this->sign ^ other.sign};
        return false;
    }

    constexpr static BigDecimal handle_rounding(UnscaledValue_t v, Exponent_t e, UnscaledValue_t rem, Sign sign, RoundingMode m) noexcept {
        switch (m) {
            case RoundingMode::Floor:
                return BigDecimal{v, e, sign};
            case RoundingMode::Ceil:
                return BigDecimal{v + 1, e, sign};
            case RoundingMode::Round:
                if (rem >= 5)
                    return BigDecimal{v + 1, e, sign};
                else
                    return BigDecimal{v, e, sign};
            default:
                assert(false);
                __builtin_unreachable();
        }
    }

    template<OverflowMode m>
    constexpr bool div(const BigDecimal &other, Exponent_t max_scale_increase, RoundingMode mode, BigDecimal &result) const noexcept {
        if (this->unscaled_value == 0) {
            result = BigDecimal{0, 0};
            return false;
        }
        UnscaledValue_t t = this->unscaled_value;
        Exponent_t ex = this->exponent;
        UnscaledValue_t div = other.unscaled_value;
        if (ex >= other.exponent) {
            if (sub_checked<m>(ex, other.exponent, ex))
                return true;
        } else {
            Exponent_t tmp{0};
            if (sub_checked<m>(other.exponent, ex, tmp))
                return true;
            if (pow_checked<m>(Exponent_t{base}, tmp, tmp))
                return true;
            if (mul_checked<m>(t, UnscaledValue_t{tmp}, t))
                return true;
            ex = 0;
        }
        UnscaledValue_t res = t / div;
        UnscaledValue_t rem = t % div;
        Sign s = this->sign ^ other.sign;
        while (rem != 0) {
            if (max_scale_increase == 0) {
                if (mul_checked<m>(rem, UnscaledValue_t{base}, rem))
                    return true;
                result = handle_rounding(res, ex, rem / div, s, mode);
                return false;
            }
            if constexpr (std::is_integral_v<Exponent_t>) {
                if (ex == std::numeric_limits<Exponent_t>::max()) {
                    if (mul_checked<m>(rem, UnscaledValue_t{base}, rem))
                        return true;
                    result = handle_rounding(res, ex, rem / div, s, mode);
                    return false;
                }
            }
            ++ex;
            if (mul_checked<m>(res, UnscaledValue_t{base}, res))
                return true;  // TODO check if we can round here instead
            if (mul_checked<m>(rem, UnscaledValue_t{base}, rem))
                return true;
            res += rem / div;
            rem = rem % div;
            --max_scale_increase;
        }
        result = BigDecimal{res, ex, s};
        return false;
    }

    template<OverflowMode m>
    constexpr bool pow(unsigned int n, BigDecimal &result) const noexcept {
        BigDecimal r{1};

        for (unsigned int i = 0; i < n; ++i) {
            if (r.mul<m>(*this, r))
                return true;
        }
        result = r;
        return false;
    }

public:
    /**
     * unary minus of this BigDecimal.
     * @return
     */
    [[nodiscard]] constexpr BigDecimal operator-() const noexcept {
        return BigDecimal(this->unscaled_value, this->exponent, !this->sign);
    }

    /**
     * unary plus (nop) of this BigDecimal.
     * @return
     */
    [[nodiscard]] constexpr BigDecimal operator+() const noexcept {
        return *this;
    }

    /**
     * addition of two BigDecimals.
     * may overflow.
     * @param other
     * @return
     */
    [[nodiscard]] constexpr BigDecimal operator+(const BigDecimal &other) const noexcept {
        BigDecimal res{0};
        add<OverflowMode::UndefinedBehavior>(other, res);
        return res;
    }
    /**
     * addition of two BigDecimals.
     * checks overflow.
     * @param other
     * @return
     */
    [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> add_checked(const BigDecimal &other) const noexcept {
        BigDecimal res{0};
        if (add<OverflowMode::Checked>(other, res))
            return nonstd::make_unexpected(DecimalError::Overflow);
        return res;
    }

    /**
     * subtraction of two BigDecimals.
     * may overflow.
     * @param other
     * @return
     * @throw std::overflow_error on over/underflow
     */
    [[nodiscard]] constexpr BigDecimal operator-(const BigDecimal &other) const noexcept {
        return *this + (-other);
    }

    /**
     * subtraction of two BigDecimals.
     * checks overflow.
     * @param other
     * @return
     * @throw std::overflow_error on over/underflow
     */
    [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> sub_checked(const BigDecimal &other) const noexcept {
        return this->add_checked(-other);
    }

    /**
     * multiplication of two BigDecimals.
     * may overflow.
     * @param other
     * @return
     */
    [[nodiscard]] constexpr BigDecimal operator*(const BigDecimal &other) const noexcept {
        BigDecimal res{0};
        mul<OverflowMode::UndefinedBehavior>(other, res);
        return res;
    }

    /**
     * multiplication of two BigDecimals.
     * checks overflow.
     * @param other
     * @return
     */
    [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> mul_checked(const BigDecimal &other) const noexcept {
        BigDecimal res{0};
        if (mul<OverflowMode::Checked>(other, res))
            return nonstd::make_unexpected(DecimalError::Overflow);
        return res;
    }

    /**
     * division of two BigDecimals.
     * may overflow.
     * @param other
     * @return
     */
    [[nodiscard]] constexpr BigDecimal operator/(const BigDecimal &other) const noexcept {
        return div(other, 1000);
    }

    /**
     * division of two BigDecimals.
     * may overflow.
     * @param other
     * @param max_scale_increase after this many decimal places have been added, stop dividing and round
     * @param mode rounding mode to use
     * @return
     */
    [[nodiscard]] constexpr BigDecimal div(const BigDecimal &other, Exponent_t max_scale_increase, RoundingMode mode = RoundingMode::Floor) const noexcept {
        if (other.unscaled_value == 0)
            return BigDecimal{0, 0};  // undefined behavior (cpp_int throws)
        BigDecimal res{0};
        div<OverflowMode::UndefinedBehavior>(other, max_scale_increase, mode, res);
        return res;
    }

    /**
     * division of two BigDecimals.
     * checks overflow.
     * @param other
     * @param max_scale_increase after this many decimal places have been added, stop dividing and round
     * @param mode rounding mode to use
     * @return
     */
    [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> div_checked(const BigDecimal &other, Exponent_t max_scale_increase, RoundingMode mode = RoundingMode::Floor) const noexcept {
        if (other.unscaled_value == 0)
            return nonstd::make_unexpected(DecimalError::NotDefined);
        BigDecimal res{0};
        if (div<OverflowMode::Checked>(other, max_scale_increase, mode, res))
            return nonstd::make_unexpected(DecimalError::Overflow);
        return res;
    }

    /**
     * raises a BigDecimal to the power of a int.
     * may overflow.
     * @param n
     * @return
     */
    [[nodiscard]] constexpr BigDecimal pow(int n) const noexcept {
        BigDecimal r{0};
        pow<OverflowMode::UndefinedBehavior>(n, r);
        return r;
    }

    /**
     * raises a BigDecimal to the power of a int.
     * checks overflow.
     * @param n
     * @return
     */
    [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> pow_checked(int n) const noexcept {
        BigDecimal r{0};
        if (pow<OverflowMode::Checked>(n, r))
            return nonstd::make_unexpected(DecimalError::Overflow);
        return r;
    }

    /**
     * rounds a BigDecimal with a specified RoundingMode.
     * @param mode
     * @return
     */
    [[nodiscard]] constexpr BigDecimal round(RoundingMode mode) const noexcept {
        if (exponent == 0)
            return *this;
        UnscaledValue_t v{base};
        if (pow_checked<OverflowMode::Checked>(v, exponent, v))
            return BigDecimal{0, 0};  // base pow exponent overflows and this did not, we have to be close to 0
        UnscaledValue_t rem = unscaled_value % v;
        rem = rem * 10 / v;
        v = unscaled_value / v;
        return handle_rounding(v, 0, rem, sign, mode);
    }

    /**
     * the absolute value of a BigDecimal.
     * @return
     */
    [[nodiscard]] constexpr BigDecimal abs() const noexcept {
        return BigDecimal{unscaled_value, exponent};
    }

    /**
     * comparison between BigDecimals.
     * @param other
     * @return
     */
    constexpr std::strong_ordering operator<=>(const BigDecimal &other) const noexcept {
        if (this->sign != other.sign)
            return this->sign == Sign::Negative ? std::strong_ordering::less : std::strong_ordering::greater;
        UnscaledValue_t t = this->unscaled_value;
        UnscaledValue_t o = other.unscaled_value;
        if (this->exponent > other.exponent) {
            UnscaledValue_t b{base};
            if (pow_checked<OverflowMode::Checked>(b, this->exponent - other.exponent, b))
                return std::strong_ordering::less;  // t does fit into the same precision, while o does not
            if (mul_checked<OverflowMode::Checked>(o, b, o))
                return std::strong_ordering::less;  // t does fit into the same precision, while o does not
        } else if (this->exponent < other.exponent) {
            UnscaledValue_t b{base};
            if (pow_checked<OverflowMode::Checked>(b, other.exponent - this->exponent, b))
                return std::strong_ordering::greater;  // o does fit into the same precision, while t does not
            if (mul_checked<OverflowMode::Checked>(t, b, t))
                return std::strong_ordering::greater;  // o does fit into the same precision, while t does not
        }
        if (t < o)
            return std::strong_ordering::less;
        else if (t > o)
            return std::strong_ordering::greater;
        else
            return std::strong_ordering::equivalent;
    };
    /**
     * equality between BigDecimals.
     * @param other
     * @return
     */
    constexpr bool operator==(const BigDecimal &other) const noexcept {
        return *this <=> other == std::strong_ordering::equivalent;
    }
    /**
     * equality between a BigDecimal and a int (mainly for constants)
     * @param t
     * @param other
     * @return
     */
    friend bool operator==(const BigDecimal &t, int other) noexcept {
        return t == BigDecimal{other, 0};
    }
    /**
     * equality between a BigDecimal and a int (mainly for constants)
     * @param t
     * @param other
     * @return
     */
    friend bool operator==(int t, const BigDecimal &other) noexcept {
        return other == t;
    }

    /**
     * conversion to a double
     * @return
     */
    [[nodiscard]] constexpr explicit operator double() const noexcept {
        double v = static_cast<double>(unscaled_value) * std::pow(static_cast<double>(base), -static_cast<double>(exponent));
        v = sign == Sign::Negative ? -v : v;
        if (!std::isnan(v) && !std::isinf(v))
            return v;
        // even Javas BigDecimal has no better solution
        auto s = static_cast<std::string>(*this);
        return std::stod(s, nullptr);
    }

    /**
     * conversion to a float
     * @return
     */
    [[nodiscard]] constexpr explicit operator float() const noexcept {
        return static_cast<float>(static_cast<double>(*this));
    }

    /**
     * conversion to a UnscaledValue_t
     * @return
     */
    [[nodiscard]] constexpr explicit operator UnscaledValue_t() const noexcept {
        if (exponent == 0)
            return unscaled_value;
        return round(RoundingMode::Floor).unscaled_value;
    }

    /**
     * conversion to a string
     * @return
     */
    [[nodiscard]] explicit operator std::string() const noexcept {
        if (unscaled_value == 0)
            return "0.0";
        std::stringstream s{};
        UnscaledValue_t v = unscaled_value;
        Exponent_t ex = exponent;
        bool hasDot = false;
        while (v != 0) {
            if (!hasDot && ex == 0) {
                if (s.view().empty())
                    s << '0';
                s << '.';
                hasDot = true;
            } else {
                --ex;
            }
            auto c = static_cast<uint32_t>(v % base);
            if (!(!hasDot && c == 0 && s.view().empty()))  // skip trailing 0s
                s << c;
            v /= base;
        }
        if (!hasDot) {
            for (Exponent_t i = 0; i < ex; ++i)
                s << '0';
            s << ".0";
        }
        if (sign == Sign::Negative)
            s << '-';
        std::string_view sv = s.view();
        return std::string{sv.rbegin(), sv.rend()};
    }

    /**
     * writing a BigDecimal into a stream.
     * @param str
     * @param bn
     * @return
     */
    friend std::ostream &operator<<(std::ostream &str, const BigDecimal &bn) {
        auto s = static_cast<std::string>(bn);
        str << s;
        return str;
    }

    /**
     * combined hash of a BigDecimals components hashes.
     * @return
     */
    [[nodiscard]] size_t hash() const {
        return (sign == Sign::Negative ? 1 : 0) ^ std::hash<UnscaledValue_t>{}(unscaled_value) ^ std::hash<Exponent_t>{}(exponent);
    }
};

template<class UnscaledValue_t, class Exponent_t>
std::string to_string(const BigDecimal<UnscaledValue_t, Exponent_t> &r) noexcept {
    return static_cast<std::string>(r);
}

template<class UnscaledValue_t, class Exponent_t>
BigDecimal<UnscaledValue_t, Exponent_t> pow(const BigDecimal<UnscaledValue_t, Exponent_t> &r, unsigned int n) noexcept {
    return r.pow(n);
}
}  // namespace rdf4cpp::rdf::util
template<class UnscaledValue_t, class Exponent_t>
struct std::hash<rdf4cpp::rdf::util::BigDecimal<UnscaledValue_t, Exponent_t>> {
    size_t operator()(const rdf4cpp::rdf::util::BigDecimal<UnscaledValue_t, Exponent_t> &r) const {
        return r.hash();
    }
};
template<class UnscaledValue_t, class Exponent_t>
class std::numeric_limits<rdf4cpp::rdf::util::BigDecimal<UnscaledValue_t, Exponent_t>> {
public:
    using BigDecimal = rdf4cpp::rdf::util::BigDecimal<UnscaledValue_t, Exponent_t>;

    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = true;
    static constexpr bool has_infinity = false;
    static constexpr bool has_quiet_NaN = false;
    static constexpr bool has_signaling_NaN = false;
    static constexpr std::float_denorm_style has_denorm = std::denorm_absent;
    static constexpr bool has_denorm_loss = false;
    static constexpr std::float_round_style round_style = std::round_toward_zero;
    static constexpr bool is_iec559 = false;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;
    static constexpr int digits = numeric_limits<UnscaledValue_t>::digits;
    static constexpr int digits10 = numeric_limits<UnscaledValue_t>::digits10;
    static constexpr int max_digits10 = numeric_limits<UnscaledValue_t>::max_digits10;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr BigDecimal max() noexcept {
        return BigDecimal{numeric_limits<UnscaledValue_t>::max(), 0};
    }
    static constexpr BigDecimal min() noexcept {
        return BigDecimal{numeric_limits<UnscaledValue_t>::max(), 0, rdf4cpp::rdf::util::Sign::Negative};
    }
    static constexpr BigDecimal lowest() noexcept {
        return min();
    }
    static constexpr BigDecimal epsilon() noexcept {
        return BigDecimal{1, numeric_limits<Exponent_t>::max()};
    }
    static constexpr BigDecimal round_error() noexcept {
        return BigDecimal{0};
    }
    static constexpr BigDecimal infinity() noexcept {
        return 0;
    }
    static constexpr BigDecimal quiet_NaN() noexcept {
        return 0;
    }
    static constexpr BigDecimal signaling_NaN() noexcept {
        return 0;
    }
    static constexpr BigDecimal denorm_min() noexcept {
        return 0;
    }
};

#endif  //RDF4CPP_BIGDECIMAL_H
