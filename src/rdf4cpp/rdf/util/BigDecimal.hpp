#ifndef RDF4CPP_BIGDECIMAL_H
#define RDF4CPP_BIGDECIMAL_H

#include <functional>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::util {
enum class RoundingMode {
    ThrowInstead,
    Floor,
    Ceil,
    Round,
};

enum class Sign {
    Positive = 0,
    Negative = 1,
};
constexpr Sign operator^(Sign a, Sign b) {
    using T = std::underlying_type_t<Sign>;
    return static_cast<Sign>(static_cast<T>(static_cast<T>(a) ^ static_cast<T>(b)));
}
constexpr Sign operator!(Sign a) {
    using T = std::underlying_type_t<Sign>;
    return static_cast<Sign>(static_cast<T>(!static_cast<T>(a)));
}

template<typename T>
concept BigDecimalBaseType = (std::unsigned_integral<T> && sizeof(T) >= sizeof(int32_t)) || std::same_as<T, boost::multiprecision::cpp_int>;

template<BigDecimalBaseType UnscaledValue_t = boost::multiprecision::cpp_int, BigDecimalBaseType Exponent_t = uint32_t>
class BigDecimal {
    UnscaledValue_t unscaled_value;
    Exponent_t exponent;
    Sign sign = Sign::Positive;

    static_assert(!std::is_integral_v<boost::multiprecision::cpp_int>);

    static constexpr uint32_t base = 10;

    /**
     * pow(base, ex) * v
     */
    static constexpr UnscaledValue_t shift_pow(UnscaledValue_t v, Exponent_t ex) {
        for (Exponent_t i = 0; i < ex; ++i)
            v = save_mul(v, UnscaledValue_t{base}, "shift_pow precision overflow");
        return v;
    }

    template<class T>
    static constexpr auto save_pow(const T &a, unsigned int b, const char *exc) {
        if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
            T r = 1;
            for (unsigned int i = 0; i < b; ++i)
                r = save_mul(r, a, exc);
            return r;
        } else {
            return boost::multiprecision::pow(a, b);
        }
    }

    template<class T>
    static constexpr auto save_add(const T &a, const T &b, const char *exc) {
        if constexpr (std::is_integral_v<T>) {
            T result;
            if (__builtin_add_overflow(a, b, &result))
                throw std::overflow_error{exc};
            return result;
        } else {
            return a + b;
        }
    }
    template<class T>
    static constexpr auto save_sub(const T &a, const T &b, const char *exc) {
        if constexpr (std::is_integral_v<T>) {
            T result;
            if (__builtin_sub_overflow(a, b, &result))
                throw std::overflow_error{exc};
            return result;
        } else {
            return a - b;
        }
    }
    template<class T>
    static constexpr auto save_mul(const T &a, const T &b, const char *exc) {
        if constexpr (std::is_integral_v<T>) {
            T result;
            if (__builtin_mul_overflow(a, b, &result))
                throw std::overflow_error{exc};
            return result;
        } else {
            return a * b;
        }
    }

    static constexpr UnscaledValue_t abs(const UnscaledValue_t &value) {
        if constexpr (std::is_integral_v<UnscaledValue_t>) {
            return std::abs(value);
        } else {
            return value < 0 ? -value : value;
        }
    }

    /**
     * sign logic for ctor
     */
    static constexpr Sign init_sign(const UnscaledValue_t &unscaled_value, Sign s) {
        if (unscaled_value == 0)
            return Sign::Positive;
        if (!std::is_integral_v<UnscaledValue_t> && unscaled_value < 0)  // only possible with cpp_int
            return !s;
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
    constexpr BigDecimal(const UnscaledValue_t &unscaled_value, Exponent_t exponent, Sign sign = Sign::Positive)
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
            Exponent_t n = c - '0';
            unscaled_value = shift_pow(unscaled_value, 1) + n;
            if (decimal)
                exponent = save_add(exponent, 1u, "ctor exponent overflow");
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
    constexpr explicit BigDecimal(int32_t value) : BigDecimal(UnscaledValue_t{std::abs(value)}, 0, value < 0 ? Sign::Negative : Sign::Positive) {}

    /**
     * converts a int64_t to a BigDecimal
     * @param value
     */
    constexpr explicit BigDecimal(int64_t value) : BigDecimal(UnscaledValue_t{std::abs(value)}, 0, value < 0 ? Sign::Negative : Sign::Positive) {}

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
            UnscaledValue_t e = save_pow(UnscaledValue_t{5}, -ex, exc);
            unscaled_value = save_mul(UnscaledValue_t{significand}, e, exc);
        } else {
            UnscaledValue_t e = save_pow(UnscaledValue_t{2}, ex, exc);
            unscaled_value = save_mul(UnscaledValue_t{significand}, e, exc);
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

    /**
     * unary minus of this BigDecimal.
     * @return
     */
    constexpr BigDecimal operator-() const {
        return BigDecimal(this->unscaled_value, this->exponent, !this->sign);
    }

    /**
     * unary plus (nop) of this BigDecimal.
     * @return
     */
    constexpr BigDecimal operator+() const noexcept {
        return *this;
    }

    /**
     * addition of two BigDecimals.
     * @param other
     * @return
     * @throw std::overflow_error on over/underflow
     */
    constexpr BigDecimal operator+(const BigDecimal &other) const {
        UnscaledValue_t t = this->unscaled_value;
        UnscaledValue_t o = other.unscaled_value;
        Exponent_t new_exp = std::max(this->exponent, other.exponent);
        if (this->exponent < new_exp) {
            t = shift_pow(t, new_exp - this->exponent);
        } else if (other.exponent < new_exp) {
            o = shift_pow(o, new_exp - other.exponent);
        }
        UnscaledValue_t res = 0;
        Sign s = Sign::Positive;
        if (this->sign == other.sign) {
            res = save_add(t, o, "BigDecimal::operator+ unscaled overflow");
            s = this->sign;
        } else if (t > o) {
            res = save_sub(t, o, "BigDecimal::operator+ unscaled overflow");
            s = this->sign;
        } else if (t < o) {
            res = save_sub(o, t, "BigDecimal::operator+ unscaled overflow");
            s = other.sign;
        }
        return BigDecimal{res, new_exp, s};
    }

    /**
     * subtraction of two BigDecimals.
     * @param other
     * @return
     * @throw std::overflow_error on over/underflow
     */
    constexpr BigDecimal operator-(const BigDecimal &other) const {
        return *this + (-other);
    }

    /**
     * multiplication of two BigDecimals.
     * @param other
     * @return
     * @throw std::overflow_error on over/underflow
     */
    constexpr BigDecimal operator*(const BigDecimal &other) const {
        return BigDecimal{save_mul(this->unscaled_value, other.unscaled_value, "BigDecimal::operator* unscaled overflow"),
                          save_add(this->exponent, other.exponent, "BigDecimal::operator* exponent overflow"), this->sign ^ other.sign};
    }

private:
    constexpr static BigDecimal handle_rounding(UnscaledValue_t v, Exponent_t e, UnscaledValue_t rem, Sign sign, RoundingMode m) {
        switch (m) {
            case RoundingMode::ThrowInstead:
                throw std::overflow_error{"division precision overflow"};
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
                throw std::logic_error{"invalid rounding mode"};
        }
    }

public:
    /**
     * division of two BigDecimals.
     * @param other
     * @param max_scale_increase after this many decimal places have been added, stop dividing and round
     * @param mode rounding mode to use
     * @return
     * @throw std::overflow_error on over/underflow, or on break to round with RoundingMode::ThrowInstead
     * @throw std::invalid_argument on trying to divide by zero
     */
    [[nodiscard]] constexpr BigDecimal divide(const BigDecimal &other, Exponent_t max_scale_increase, RoundingMode mode = RoundingMode::ThrowInstead) const {
        if (other.unscaled_value == 0)
            throw std::invalid_argument{"division by 0"};
        if (this->unscaled_value == 0)
            return BigDecimal{0, 0};
        UnscaledValue_t t = this->unscaled_value;
        Exponent_t ex = this->exponent;
        UnscaledValue_t div = other.unscaled_value;
        if (ex >= other.exponent) {
            ex -= other.exponent;
        } else {
            t = shift_pow(t, other.exponent - ex);
            ex = 0;
        }
        UnscaledValue_t res = t / div;
        UnscaledValue_t rem = t % div;
        Sign s = this->sign ^ other.sign;
        while (rem != 0) {
            if (max_scale_increase == 0) {
                rem = shift_pow(rem, 1);
                return handle_rounding(res, ex, rem / div, s, mode);
            }
            if constexpr (std::is_integral_v<Exponent_t>) {
                if (ex == std::numeric_limits<Exponent_t>::max()) {
                    rem = shift_pow(rem, 1);
                    return handle_rounding(res, ex, rem / div, s, mode);
                }
            }
            ++ex;
            res = shift_pow(res, 1);
            rem = shift_pow(rem, 1);
            res += rem / div;
            rem = rem % div;
            --max_scale_increase;
        }
        return BigDecimal{res, ex, s};
    }
    /**
     * division of two BigDecimals.
     * @param other
     * @return
     * @throw std::overflow_error on over/underflow, or on hitting 1000 additional decimal places
     * @throw std::invalid_argument on trying to divide by zero
     */
    constexpr BigDecimal operator/(const BigDecimal &other) const {
        return this->divide(other, 1000);
    }

    /**
     * raises a BigDecimal to the power of a int.
     * @param n
     * @return
     * @throw std::overflow_error on over/underflow
     */
    constexpr BigDecimal pow(int n) const {
        BigDecimal r{1};
        if (n < 0) {
            for (int i = 0; i > n; --i)
                r = r / *this;
        } else {
            for (int i = 0; i < n; ++i)
                r = r * *this;
        }
        return r;
    }

    /**
     * rounds a BigDecimal with a specified RoundingMode.
     * @param mode
     * @return
     * @throw std::overflow_error on over/underflow or RoundingMode::ThrowInstead
     */
    [[nodiscard]] constexpr BigDecimal round(RoundingMode mode) const {
        if (exponent == 0)
            return *this;
        UnscaledValue_t v = unscaled_value / shift_pow(1, exponent);
        UnscaledValue_t rem = unscaled_value / shift_pow(1, exponent - 1) - v * base;
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
            try {
                o = shift_pow(o, this->exponent - other.exponent);
            } catch (const std::overflow_error &) {
                return std::strong_ordering::less;  // t does fit into the same precision, while o does not
            }
        } else if (this->exponent < other.exponent) {
            try {
                t = shift_pow(t, other.exponent - this->exponent);
            } catch (const std::overflow_error &) {
                return std::strong_ordering::greater;  // o does fit into the same precision, while t does not
            }
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
    constexpr explicit operator double() const noexcept {
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
    constexpr explicit operator float() const noexcept {
        return static_cast<float>(static_cast<double>(*this));
    }

    /**
     * conversion to a UnscaledValue_t
     * @return
     */
    constexpr explicit operator UnscaledValue_t() const noexcept {
        if (exponent == 0)
            return unscaled_value;
        return unscaled_value / shift_pow(1, exponent);
    }

    /**
     * conversion to a string
     * @return
     */
    explicit operator std::string() const noexcept {
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
BigDecimal<UnscaledValue_t, Exponent_t> pow(const BigDecimal<UnscaledValue_t, Exponent_t> &r, int n) {
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
