#ifndef RDF4CPP_BIGDECIMAL_H
#define RDF4CPP_BIGDECIMAL_H

#include <functional>
#include <sstream>
#include <string>
#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>
#include <dice/hash.hpp>

#include <rdf4cpp/Expected.hpp>

namespace rdf4cpp {
    enum struct RoundingMode {
        Floor,
        Ceil,
        Round,
        Trunc,
    };

    enum struct DecimalError {
        Overflow,
        NotDefined,  // aka NotANumber
    };

    template<typename T>
    concept BigDecimalBaseType = std::numeric_limits<T>::is_specialized && !std::floating_point<T>;

    template<BigDecimalBaseType UnscaledValue = boost::multiprecision::cpp_int, BigDecimalBaseType Exponent = uint32_t>
    requires(!std::signed_integral<Exponent> && !std::unsigned_integral<UnscaledValue>)
    struct BigDecimal {
        // the entire class is loosely based on OpenJDKs BigDecimal: https://github.com/AdoptOpenJDK/openjdk-jdk11/blob/master/src/java.base/share/classes/java/math/BigDecimal.java

    private:
        UnscaledValue unscaled_value_;
        Exponent exponent_;

        static constexpr int32_t BASE = 10;

        enum struct OverflowMode {
            CHECKED,
            UNDEFINED_BEHAVIOR,
        };

        template<OverflowMode m, typename T>
        static constexpr bool add_checked(T const &a, T const &b, T &result) noexcept {
            if constexpr (std::is_integral_v<T> && m == OverflowMode::CHECKED) {
                return __builtin_add_overflow(a, b, &result);
            } else {
                result = a + b;
                return false;
            }
        }

        template<OverflowMode m, typename T>
        static constexpr bool sub_checked(T const &a, T const &b, T &result) noexcept {
            if constexpr (std::is_integral_v<T> && m == OverflowMode::CHECKED) {
                return __builtin_sub_overflow(a, b, &result);
            } else {
                result = a - b;
                return false;
            }
        }

        template<OverflowMode m, typename T>
        static constexpr bool mul_checked(T const &a, T const &b, T &result) noexcept {
            if constexpr (std::is_integral_v<T> && m == OverflowMode::CHECKED) {
                return __builtin_mul_overflow(a, b, &result);
            } else {
                result = a * b;
                return false;
            }
        }

        template<OverflowMode m, typename T>
        static constexpr bool pow_checked(T const &a, unsigned int b, T &result) noexcept {
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

        template<OverflowMode m, typename To, typename From>
        static constexpr bool cast_checked(From const &f, To &result) noexcept {
            if constexpr (std::is_integral_v<To> && std::is_integral_v<From> && m == OverflowMode::CHECKED) {
                if (!std::in_range<To>(f))
                    return true;
            }
            result = static_cast<To>(f);
            return false;
        }

        static constexpr UnscaledValue abs(UnscaledValue const &value) noexcept {
            if constexpr (std::is_integral_v<UnscaledValue> && !std::is_signed_v<UnscaledValue>) {
                return value;
            } else {
                return value < 0 ? -value : value;
            }
        }

    public:
        constexpr BigDecimal() noexcept
            : BigDecimal{0, 0} {
        }

        /**
         * creates a BigDecimal from its components.
         * it has the value of unscaled_value * pow(10, -exponent).
         * @param unscaled_value
         * @param exponent
         */
        constexpr BigDecimal(UnscaledValue const &unscaled_value, Exponent exponent) noexcept
            : unscaled_value_(unscaled_value), exponent_(exponent) {
        }

        /**
         * parses a BigDecimal from a string_view.
         * may include a leading sign and one decimal point ., everything else needs to be numeric.
         * @param value
         * @throw std::invalid_argument if a invalid char is found
         * @throw std::overflow_error on exceeding the types numeric limits
         */
        constexpr explicit BigDecimal(std::string_view value)
            : unscaled_value_(0), exponent_(0) {
            bool begin = true;
            bool decimal = false;
            bool neg = false;
            for (char const c : value) {
                if (begin) {
                    begin = false;
                    if (c == '-') {
                        neg = true;
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
                if (mul_checked<OverflowMode::CHECKED>(unscaled_value_, UnscaledValue{BASE}, unscaled_value_))
                    throw std::overflow_error{"ctor unscaled_value overflow"};
                if (add_checked<OverflowMode::CHECKED>(unscaled_value_, UnscaledValue{n}, unscaled_value_))
                    throw std::overflow_error{"ctor unscaled_value overflow"};
                if (decimal) {
                    if (add_checked<OverflowMode::CHECKED>(exponent_, Exponent{1}, exponent_))
                        throw std::overflow_error{"ctor exponent overflow"};
                }
            }
            if (unscaled_value_ == 0)
                neg = false;
            if (neg)
                unscaled_value_ = -unscaled_value_;
        }

        /**
         * converts a uint32_t to a BigDecimal
         * @param value
         */
        constexpr explicit BigDecimal(uint32_t value) noexcept
            : BigDecimal(UnscaledValue{value}, 0) {
        }

        /**
         * converts a uint64_t to a BigDecimal
         * @param value
         */
        constexpr explicit BigDecimal(uint64_t value) noexcept
            : BigDecimal(UnscaledValue{value}, 0) {
        }

        /**
         * converts a int32_t to a BigDecimal
         * @param value
         */
        constexpr explicit BigDecimal(int32_t value) noexcept
            : BigDecimal(static_cast<UnscaledValue>(value), 0) {
        }

        /**
         * converts a int64_t to a BigDecimal
         * @param value
         */
        constexpr explicit BigDecimal(int64_t value) noexcept
            : BigDecimal(static_cast<UnscaledValue>(value), 0) {
        }

        /**
         * converts a UnscaledValue to a BigDecimal
         * @param value
         */
        constexpr explicit BigDecimal(UnscaledValue const &value) noexcept
        requires(!std::is_same_v<UnscaledValue, int32_t> && !std::is_same_v<UnscaledValue, int64_t>)
            : BigDecimal(value, 0) {
        }

        /**
         * converts a float to a BigDecimal.
         * this conversion might not be exact, due to the built in limitations of floats.
         * if you have the possibility, use one of the other constructors.
         * @param value
         * @throw std::overflow_error on exceeding the types numeric limits
         */
        constexpr explicit BigDecimal(float value)
            : BigDecimal(static_cast<double>(value)) {
        }

        /**
         * converts a double to a BigDecimal.
         * this conversion might not be exact, due to the built in limitations of doubles.
         * if you have the possibility, use one of the other constructors.
         * @param value
         * @throw std::overflow_error on exceeding the types numeric limits
         */
        explicit BigDecimal(double value)
            : unscaled_value_(0), exponent_(0) {
            // NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers)
            // most of the algorithm is from OpenJDK: https://github.com/AdoptOpenJDK/openjdk-jdk11/blob/19fb8f93c59dfd791f62d41f332db9e306bc1422/src/java.base/share/classes/java/math/BigDecimal.java#L915
            if (std::isinf(value) || std::isnan(value))
                throw std::invalid_argument{"value is NaN or infinity"};
            // this might fail on anything that is not x86-32/64
            static_assert(std::endian::native == std::endian::little, "BigDecimal{double} is only tested on x86-32/64 and might not work on other systems");
            // double is an IEEE 754 64-bit floating point value
            // memory layout:
            // sign | exponent  | fraction
            // 63   | 62 ... 52 | 51 ... 0
            // see https://en.wikipedia.org/wiki/Double-precision_floating-point_format for more info (and a better graphic)
            auto v = std::bit_cast<uint64_t>(value);
            bool neg = (v >> 63) != 0;
            auto ex = static_cast<int>((v >> 52) & 0x7ffL);
            uint64_t significand = ex == 0
                                           ? (v & ((1L << 52) - 1)) << 1
                                           : (v & ((1L << 52) - 1)) | (1L << 52);
            ex -= 1075;
            if (significand == 0) {
                return;
            }
            while (significand & 1) {
                significand >>= 1;
                ++ex;
            }
            static constexpr char const *exc = "double to BigDecimal overflow";
            if (ex == 0) {
                unscaled_value_ = UnscaledValue{significand};
            } else if (ex < 0) {
                exponent_ = static_cast<Exponent>(-ex);
                UnscaledValue e{0};
                if (pow_checked<OverflowMode::CHECKED>(UnscaledValue{5}, -ex, e))
                    throw std::overflow_error{exc};
                if (mul_checked<OverflowMode::CHECKED>(UnscaledValue{significand}, e, unscaled_value_))
                    throw std::overflow_error{exc};
            } else {
                UnscaledValue e{0};
                if (pow_checked<OverflowMode::CHECKED>(UnscaledValue{2}, ex, e))
                    throw std::overflow_error{exc};
                if (mul_checked<OverflowMode::CHECKED>(UnscaledValue{significand}, e, unscaled_value_))
                    throw std::overflow_error{exc};
            }
            if (neg)
                unscaled_value_ = -unscaled_value_;
            normalize();
            // NOLINTEND (cppcoreguidelines-avoid-magic-numbers)
        }

        /**
         * converts this BigDecimal to its smallest internal representation.
         */
        constexpr void normalize() noexcept {
            while (exponent_ > 0 && unscaled_value_ % BASE == 0) {
                unscaled_value_ /= BASE;
                --exponent_;
            }
        }

        [[nodiscard]] constexpr Exponent get_exponent() const noexcept {
            return exponent_;
        }

        [[nodiscard]] constexpr UnscaledValue get_unscaled_value() const noexcept {
            return unscaled_value_;
        }

        [[nodiscard]] constexpr bool positive() const noexcept {
            return unscaled_value_ >= 0;
        }

    private:
        // op_checked has to be add_checked or sub_checked with the same OverflowMode
        template<OverflowMode m, bool (*op_checked)(UnscaledValue const &t, UnscaledValue const &o, UnscaledValue &result)>
        constexpr bool add_or_sub(BigDecimal const &other, BigDecimal &result) const noexcept {
            UnscaledValue t = this->unscaled_value_;
            UnscaledValue o = other.unscaled_value_;
            Exponent new_exp = std::max(this->exponent_, other.exponent_);
            if (this->exponent_ < new_exp) {
                UnscaledValue ex{0};
                if (pow_checked<m>(UnscaledValue{BASE}, new_exp - this->exponent_, ex))
                    return true;
                if (mul_checked<m>(t, ex, t))
                    return true;
            } else if (other.exponent_ < new_exp) {
                UnscaledValue ex{0};
                if (pow_checked<m>(UnscaledValue{BASE}, new_exp - other.exponent_, ex))
                    return true;
                if (mul_checked<m>(o, ex, o))
                    return true;
            }
            UnscaledValue res = 0;
            if (op_checked(t, o, res))
                return true;
            result = BigDecimal{res, new_exp};
            return false;
        }

        template<OverflowMode m>
        constexpr bool mul(BigDecimal const &other, BigDecimal &result) const noexcept {
            UnscaledValue v{0};
            if (mul_checked<m>(this->unscaled_value_, other.unscaled_value_, v))
                return true;
            Exponent e{0};
            if (add_checked<m>(this->exponent_, other.exponent_, e))
                return true;
            result = BigDecimal{v, e};
            return false;
        }

        static constexpr BigDecimal handle_rounding(UnscaledValue v, Exponent e, UnscaledValue rem, RoundingMode m) noexcept {
            switch (m) {
                case RoundingMode::Trunc:
                    return BigDecimal{v, e};
                case RoundingMode::Floor:
                    if (v >= 0 || rem == 0)
                        return BigDecimal{v, e};
                    else
                        return BigDecimal{v - 1, e};
                case RoundingMode::Ceil:
                    if (v < 0 || rem == 0)
                        return BigDecimal{v, e};
                    else
                        return BigDecimal{v + 1, e};
                case RoundingMode::Round:
                    if (rem < 0)
                        rem = -rem;
                    if (v < 0) {
                        if (rem >= BASE / 2)
                            return BigDecimal{v - 1, e};
                        else
                            return BigDecimal{v, e};
                    } else {
                        if (rem >= BASE / 2)
                            return BigDecimal{v + 1, e};
                        else
                            return BigDecimal{v, e};
                    }
                default:
                    assert(false);
                    __builtin_unreachable();
            }
        }

        template<OverflowMode m>
        constexpr bool div(BigDecimal const &other, Exponent max_scale_increase, RoundingMode mode, BigDecimal &result) const noexcept {
            if (this->unscaled_value_ == 0) {
                result = BigDecimal{0, 0};
                return false;
            }
            UnscaledValue t = this->unscaled_value_;
            Exponent ex = this->exponent_;
            UnscaledValue div = other.unscaled_value_;
            if (ex >= other.exponent_) {
                if (sub_checked<m>(ex, other.exponent_, ex))
                    return true;
            } else {
                Exponent tmp{0};
                if (sub_checked<m>(other.exponent_, ex, tmp))
                    return true;
                if (pow_checked<m>(Exponent{BASE}, tmp, tmp))
                    return true;
                UnscaledValue tmp2{0};
                if (cast_checked<m>(tmp, tmp2))
                    return true;
                if (mul_checked<m>(t, tmp2, t))
                    return true;
                ex = 0;
            }
            UnscaledValue res = t / div;
            UnscaledValue rem = t % div;
            while (rem != 0) {
                if (max_scale_increase == 0) {
                    if (mul_checked<m>(rem, UnscaledValue{BASE}, rem))
                        return true;
                    result = handle_rounding(res, ex, rem / div, mode);
                    return false;
                }
                if constexpr (std::is_integral_v<Exponent>) {
                    if (ex == std::numeric_limits<Exponent>::max()) {
                        if (mul_checked<m>(rem, UnscaledValue{BASE}, rem))
                            return true;
                        result = handle_rounding(res, ex, rem / div, mode);
                        return false;
                    }
                }
                ++ex;
                if (mul_checked<m>(res, UnscaledValue{BASE}, res))
                    return true;
                if (mul_checked<m>(rem, UnscaledValue{BASE}, rem))
                    return true;
                res += rem / div;
                rem = rem % div;
                --max_scale_increase;
            }
            result = BigDecimal{res, ex};
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
         * may overflow.
         * @return
         */
        [[nodiscard]] constexpr BigDecimal operator-() const noexcept {
            return BigDecimal(-this->unscaled_value_, this->exponent_);
        }
        /**
         * unary minus of this BigDecimal.
         * checks overflow.
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> unary_minus_checked() const noexcept {
            if constexpr (std::is_integral_v<UnscaledValue>) {
                if (std::numeric_limits<UnscaledValue>::min() == unscaled_value_)
                    return nonstd::make_unexpected(DecimalError::Overflow);
            }
            return BigDecimal(-this->unscaled_value_, this->exponent_);
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
        [[nodiscard]] constexpr BigDecimal operator+(BigDecimal const &other) const noexcept {
            BigDecimal res{0};
            add_or_sub<OverflowMode::UNDEFINED_BEHAVIOR, add_checked<OverflowMode::UNDEFINED_BEHAVIOR, UnscaledValue>>(other, res);
            return res;
        }
        /**
         * addition of two BigDecimals.
         * checks overflow.
         * @param other
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> add_checked(BigDecimal const &other) const noexcept {
            BigDecimal res{0};
            if (add_or_sub<OverflowMode::CHECKED, add_checked<OverflowMode::CHECKED, UnscaledValue>>(other, res))
                return nonstd::make_unexpected(DecimalError::Overflow);
            return res;
        }

        /**
         * addition of two BigDecimals.
         * may overflow.
         * @param other
         * @return
         */
        constexpr BigDecimal &operator+=(BigDecimal const &other) noexcept {
            *this = *this + other;
            return *this;
        }

        /**
         * subtraction of two BigDecimals.
         * may overflow.
         * @param other
         * @return
         */
        [[nodiscard]] constexpr BigDecimal operator-(BigDecimal const &other) const noexcept {
            BigDecimal res{0};
            add_or_sub<OverflowMode::UNDEFINED_BEHAVIOR, sub_checked<OverflowMode::UNDEFINED_BEHAVIOR, UnscaledValue>>(other, res);
            return res;
        }

        /**
         * subtraction of two BigDecimals.
         * checks overflow.
         * @param other
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> sub_checked(BigDecimal const &other) const noexcept {
            BigDecimal res{0};
            if (add_or_sub<OverflowMode::CHECKED, sub_checked<OverflowMode::CHECKED, UnscaledValue>>(other, res))
                return nonstd::make_unexpected(DecimalError::Overflow);
            return res;
        }

        /**
         * subtraction of two BigDecimals.
         * may overflow.
         * @param other
         * @return
         */
        constexpr BigDecimal operator-=(BigDecimal const &other) noexcept {
            *this = *this - other;
            return *this;
        }

        /**
         * multiplication of two BigDecimals.
         * may overflow.
         * @param other
         * @return
         */
        [[nodiscard]] constexpr BigDecimal operator*(BigDecimal const &other) const noexcept {
            BigDecimal res{0};
            mul<OverflowMode::UNDEFINED_BEHAVIOR>(other, res);
            return res;
        }

        /**
         * multiplication of two BigDecimals.
         * checks overflow.
         * @param other
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> mul_checked(BigDecimal const &other) const noexcept {
            BigDecimal res{0};
            if (mul<OverflowMode::CHECKED>(other, res))
                return nonstd::make_unexpected(DecimalError::Overflow);
            return res;
        }

        /**
         * multiplication of two BigDecimals.
         * may overflow.
         * @param other
         * @return
         */
        constexpr BigDecimal &operator*=(BigDecimal const &other) noexcept {
            *this = *this * other;
            return *this;
        }

        /**
         * division of two BigDecimals.
         * may overflow.
         * after 1000 decimal places have been added, stops dividing and floor.
         * dividing by 0 is undefined behavior.
         * @param other
         * @return
         */
        [[nodiscard]] constexpr BigDecimal operator/(BigDecimal const &other) const noexcept {
            return div(other, 1000);
        }

        /**
         * division of two BigDecimals.
         * may overflow.
         * dividing by 0 is undefined behavior.
         * @param other
         * @param max_scale_increase after this many decimal places have been added, stop dividing and round
         * @param mode rounding mode to use
         * @return
         */
        [[nodiscard]] constexpr BigDecimal div(BigDecimal const &other, Exponent max_scale_increase, RoundingMode mode = RoundingMode::Floor) const noexcept {
            if (other.unscaled_value_ == 0)
                return BigDecimal{0, 0};  // undefined behavior (cpp_int throws)
            BigDecimal res{0};
            div<OverflowMode::UNDEFINED_BEHAVIOR>(other, max_scale_increase, mode, res);
            return res;
        }

        /**
         * division of two BigDecimals.
         * checks overflow and division by 0.
         * @param other
         * @param max_scale_increase after this many decimal places have been added, stop dividing and round
         * @param mode rounding mode to use
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> div_checked(BigDecimal const &other, Exponent max_scale_increase, RoundingMode mode = RoundingMode::Floor) const noexcept {
            if (other.unscaled_value_ == 0)
                return nonstd::make_unexpected(DecimalError::NotDefined);
            BigDecimal res{0};
            if (div<OverflowMode::CHECKED>(other, max_scale_increase, mode, res))
                return nonstd::make_unexpected(DecimalError::Overflow);
            return res;
        }

        /**
         * division of two BigDecimals.
         * may overflow.
         * after 1000 decimal places have been added, stops dividing and floor.
         * dividing by 0 is undefined behavior.
         * @param other
         * @return
         */
        constexpr BigDecimal &operator/=(BigDecimal const &other) noexcept {
            *this = *this / other;
            return *this;
        }

        /**
         * raises a BigDecimal to the power of a int.
         * may overflow.
         * @param n
         * @return
         */
        [[nodiscard]] constexpr BigDecimal pow(unsigned int n) const noexcept {
            BigDecimal r{0};
            pow<OverflowMode::UNDEFINED_BEHAVIOR>(n, r);
            return r;
        }

        /**
         * raises a BigDecimal to the power of a int.
         * checks overflow.
         * @param n
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> pow_checked(unsigned int n) const noexcept {
            BigDecimal r{0};
            if (pow<OverflowMode::CHECKED>(n, r))
                return nonstd::make_unexpected(DecimalError::Overflow);
            return r;
        }

        /**
         * rounds a BigDecimal with a specified RoundingMode.
         * @param mode
         * @return
         */
        [[nodiscard]] constexpr BigDecimal round(RoundingMode mode) const noexcept {
            if (exponent_ == 0)
                return *this;
            UnscaledValue v{BASE};
            if (pow_checked<OverflowMode::CHECKED>(v, exponent_, v))
                return BigDecimal{0, 0};  // base pow exponent overflows and this did not, we have to be close to 0
            UnscaledValue rem = unscaled_value_ % v;
            rem = rem * BASE / v;
            v = unscaled_value_ / v;
            return handle_rounding(v, 0, rem, mode);
        }

        /**
         * the absolute value of a BigDecimal.
         * may overflow.
         * @return
         */
        [[nodiscard]] constexpr BigDecimal abs() const noexcept {
            if (positive())
                return *this;
            else
                return -*this;
        }

        /**
         * the absolute value of a BigDecimal.
         * checks overflow.
         * @return
         */
        [[nodiscard]] constexpr nonstd::expected<BigDecimal, DecimalError> abs_checked() const noexcept {
            if (positive())
                return *this;
            else
                return this->unary_minus_checked();
        }

        /**
         * comparison between BigDecimals.
         * @param other
         * @return
         */
        constexpr std::strong_ordering operator<=>(BigDecimal const &other) const noexcept {
            if (this->positive() != other.positive())
                return this->positive() ? std::strong_ordering::greater : std::strong_ordering::less;
            UnscaledValue t = this->unscaled_value_;
            UnscaledValue o = other.unscaled_value_;
            if (this->exponent_ > other.exponent_) {
                UnscaledValue b{BASE};
                if (pow_checked<OverflowMode::CHECKED>(b, this->exponent_ - other.exponent_, b))
                    return std::strong_ordering::less;  // t does fit into the same precision, while o does not
                if (mul_checked<OverflowMode::CHECKED>(o, b, o))
                    return std::strong_ordering::less;  // t does fit into the same precision, while o does not
            } else if (this->exponent_ < other.exponent_) {
                UnscaledValue b{BASE};
                if (pow_checked<OverflowMode::CHECKED>(b, other.exponent_ - this->exponent_, b))
                    return std::strong_ordering::greater;  // o does fit into the same precision, while t does not
                if (mul_checked<OverflowMode::CHECKED>(t, b, t))
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
        constexpr bool operator==(BigDecimal const &other) const noexcept {
            return *this <=> other == std::strong_ordering::equivalent;
        }
        /**
         * equality between a BigDecimal and a int (mainly for constants)
         * @param t
         * @param other
         * @return
         */
        friend bool operator==(BigDecimal const &t, int other) noexcept {
            return t == BigDecimal{other, 0};
        }
        /**
         * equality between a BigDecimal and a int (mainly for constants)
         * @param t
         * @param other
         * @return
         */
        friend bool operator==(int t, BigDecimal const &other) noexcept {
            return other == t;
        }

        /**
         * conversion to a double
         * @return
         */
        [[nodiscard]] explicit operator double() const noexcept {
            double const v = static_cast<double>(unscaled_value_) * std::pow(static_cast<double>(BASE), -static_cast<double>(exponent_));
            if (!std::isnan(v) && !std::isinf(v))
                return v;
            // even Javas BigDecimal has no better solution
            auto const s = static_cast<std::string>(*this);
            return std::stod(s);
        }

        /**
         * conversion to a float
         * @return
         */
        [[nodiscard]] explicit operator float() const noexcept {
            return static_cast<float>(static_cast<double>(*this));
        }

        /**
         * conversion to a UnscaledValue
         * @return
         */
        [[nodiscard]] constexpr explicit operator UnscaledValue() const noexcept {
            if (exponent_ == 0)
                return unscaled_value_;
            return round(RoundingMode::Trunc).unscaled_value_;
        }

        /**
         * conversion to a string
         * @return
         */
        [[nodiscard]] explicit operator std::string() const noexcept {
            if (unscaled_value_ == 0)
                return "0.0";
            std::stringstream s{};
            UnscaledValue v = unscaled_value_;
            Exponent ex = exponent_;
            bool has_dot = false;
            while (v != 0) {
                if (!has_dot && ex == 0) {
                    if (s.view().empty())
                        s << '0';
                    s << '.';
                    has_dot = true;
                } else {
                    --ex;
                }
                using namespace std;
                auto c = static_cast<uint32_t>(abs(v % BASE));
                if (has_dot || c != 0 || !s.view().empty())  // skip trailing 0s
                    s << c;
                v /= BASE;
            }
            if (!has_dot) {
                for (Exponent i = 0; i < ex; ++i)
                    s << '0';
                s << ".0";
            }
            if (!positive())
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
        friend std::ostream &operator<<(std::ostream &str, BigDecimal const &bn) {
            auto s = static_cast<std::string>(bn);
            str << s;
            return str;
        }

        /**
         * combined hash of a BigDecimals components hashes.
         * @return
         */
        template<dice::hash::Policies::HashPolicy Policy = dice::hash::Policies::wyhash>
        [[nodiscard]] size_t hash() const {
            BigDecimal n = *this;
            n.normalize();

            return dice::hash::dice_hash_templates<Policy>::dice_hash(std::tie(n.unscaled_value_, n.exponent_));
        }
    };

    template<typename UnscaledValue, typename Exponent>
    std::string to_string(BigDecimal<UnscaledValue, Exponent> const &r) noexcept {
        return static_cast<std::string>(r);
    }

    template<typename UnscaledValue, typename Exponent>
    BigDecimal<UnscaledValue, Exponent> pow(BigDecimal<UnscaledValue, Exponent> const &r, unsigned int n) noexcept {
        return r.pow(n);
    }
    template<typename UnscaledValue, typename Exponent>
    BigDecimal<UnscaledValue, Exponent> round(BigDecimal<UnscaledValue, Exponent> const &r) noexcept {
        return r.round(RoundingMode::Round);
    }
    template<typename UnscaledValue, typename Exponent>
    BigDecimal<UnscaledValue, Exponent> floor(BigDecimal<UnscaledValue, Exponent> const &r) noexcept {
        return r.round(RoundingMode::Floor);
    }
    template<typename UnscaledValue, typename Exponent>
    BigDecimal<UnscaledValue, Exponent> ceil(BigDecimal<UnscaledValue, Exponent> const &r) noexcept {
        return r.round(RoundingMode::Ceil);
    }
    template<typename UnscaledValue, typename Exponent>
    BigDecimal<UnscaledValue, Exponent> trunc(BigDecimal<UnscaledValue, Exponent> const &r) noexcept {
        return r.round(RoundingMode::Trunc);
    }
    template<typename UnscaledValue, typename Exponent>
    BigDecimal<UnscaledValue, Exponent> abs(BigDecimal<UnscaledValue, Exponent> const &r) noexcept {
        return r.abs();
    }
}  // namespace rdf4cpp

#ifndef DOXYGEN_PARSER
template<typename UnscaledValue, typename Exponent>
struct std::hash<rdf4cpp::BigDecimal<UnscaledValue, Exponent>> {
    size_t operator()(rdf4cpp::BigDecimal<UnscaledValue, Exponent> const &r) const {
        return r.hash();
    }
};

template<typename Policy>
struct dice::hash::dice_hash_overload<Policy, ::boost::multiprecision::cpp_int> {
    static size_t dice_hash(::boost::multiprecision::cpp_int const &x) noexcept {
        return dice::hash::dice_hash_templates<Policy>::dice_hash(std::hash<::boost::multiprecision::cpp_int>{}(x));
    }
};

template<typename Policy, typename U, typename E>
struct dice::hash::dice_hash_overload<Policy, rdf4cpp::BigDecimal<U, E>> {
    static size_t dice_hash(rdf4cpp::BigDecimal<U, E> const &x) noexcept {
        return x.template hash<Policy>();
    }
};
#endif

template<typename UnscaledValue, typename Exponent>
class std::numeric_limits<rdf4cpp::BigDecimal<UnscaledValue, Exponent>> {
public:
    using BigDecimal = rdf4cpp::BigDecimal<UnscaledValue, Exponent>;

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
    static constexpr int digits = numeric_limits<UnscaledValue>::digits;
    static constexpr int digits10 = numeric_limits<UnscaledValue>::digits10;
    static constexpr int max_digits10 = numeric_limits<UnscaledValue>::max_digits10;
    static constexpr int radix = 2;
    static constexpr int min_exponent = 0;
    static constexpr int min_exponent10 = 0;
    static constexpr int max_exponent = 0;
    static constexpr int max_exponent10 = 0;
    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr BigDecimal max() noexcept {
        return BigDecimal{numeric_limits<UnscaledValue>::max(), 0};
    }
    static constexpr BigDecimal min() noexcept {
        return BigDecimal{numeric_limits<UnscaledValue>::min(), 0};
    }
    static constexpr BigDecimal lowest() noexcept {
        return min();
    }
    static constexpr BigDecimal epsilon() noexcept {
        return BigDecimal{1, numeric_limits<Exponent>::max()};
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
