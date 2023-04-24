#ifndef RDF4CPP_BIGDECIMAL_H
#define RDF4CPP_BIGDECIMAL_H

#include <string>
#include <sstream>
#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::util {
enum class RoundingMode {
    ThrowInstead,
    Floor,
    Ceil,
    Round,
};

template<typename UnscaledValue_t = boost::multiprecision::cpp_int, typename Exponent_t = uint32_t>
class BigDecimal {
    UnscaledValue_t unscaled_value;
    Exponent_t exponent;
    bool sign;

    static_assert(!std::is_integral_v<boost::multiprecision::cpp_int>);

    static constexpr uint32_t base = 10;

    static constexpr UnscaledValue_t shift_pow(UnscaledValue_t v, Exponent_t ex) {
        for (Exponent_t i = 0; i < ex; ++i)
            v *= base;
        return v;
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

public:
    constexpr BigDecimal(UnscaledValue_t unscaled_value, Exponent_t exponent, bool sign = false) : unscaled_value(unscaled_value), exponent(exponent), sign(sign) {
    }

    constexpr void normalize() noexcept {
        while (exponent > 0 && unscaled_value % base == 0) {
            unscaled_value /= base;
            --exponent;
        }
    }

    [[nodiscard]] constexpr Exponent_t get_exponent() const noexcept {
        return exponent;
    }

    constexpr BigDecimal operator-() const {
        return BigDecimal(this->unscaled_value, this->exponent, !this->sign);
    }

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
        bool s = false;
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

    constexpr BigDecimal operator-(const BigDecimal &other) const {
        return *this + (-other);
    }

    constexpr BigDecimal operator*(const BigDecimal &other) const {
        return BigDecimal{save_mul(this->unscaled_value, other.unscaled_value, "BigDecimal::operator* unscaled overflow"),
                          save_add(this->exponent, other.exponent, "BigDecimal::operator* exponent overflow"), static_cast<bool>(this->sign ^ other.sign)};
    }

private:
    constexpr static BigDecimal handle_rounding(UnscaledValue_t v, Exponent_t e, UnscaledValue_t rem, bool sign, RoundingMode m) {
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
    constexpr BigDecimal divide(const BigDecimal &other, Exponent_t max_scale_increase, RoundingMode mode = RoundingMode::ThrowInstead) const {
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
        bool s = static_cast<bool>(this->sign ^ other.sign);
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
    constexpr BigDecimal operator/(const BigDecimal &other) const {
        return this->divide(other, 1000);
    }

    constexpr std::strong_ordering operator<=>(const BigDecimal &other) const noexcept {
        if (this->sign != other.sign)
            return this->sign == true ? std::strong_ordering::less : std::strong_ordering::greater;
        UnscaledValue_t t = this->unscaled_value;
        UnscaledValue_t o = other.unscaled_value;
        if (this->exponent > other.exponent) {
            o = shift_pow(o, this->exponent - other.exponent);
        } else if (this->exponent < other.exponent) {
            t = shift_pow(t, other.exponent - this->exponent);
        }
        if (t < o)
            return std::strong_ordering::less;
        else if (t > o)
            return std::strong_ordering::greater;
        else
            return std::strong_ordering::equivalent;
    };
    constexpr bool operator==(const BigDecimal &other) const noexcept {
        return *this <=> other == std::strong_ordering::equivalent;
    }

    constexpr explicit operator double() const noexcept {
        double v = static_cast<double>(unscaled_value) * std::pow(static_cast<double>(base), -static_cast<double>(exponent));
        return sign ? -v : v;
    }

    explicit operator std::string() const noexcept {
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
            s << c;
            v /= base;
        }
        if (!hasDot) {
            for (Exponent_t i = 0; i < ex; ++i)
                s << '0';
            s << ".0";
        }
        if (sign)
            s << '-';
        std::string_view sv = s.view();
        return std::string{sv.rbegin(), sv.rend()};
    }

    friend std::ostream &operator<<(std::ostream &str, const BigDecimal &bn) {
        auto s = static_cast<std::string>(bn);
        str << s;
        return str;
    }
};
}  // namespace rdf4cpp::rdf::util

#endif  //RDF4CPP_BIGDECIMAL_H
