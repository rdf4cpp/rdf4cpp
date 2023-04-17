#ifndef RDF4CPP_BIGDECIMAL_H
#define RDF4CPP_BIGDECIMAL_H

#include <string>
#include <sstream>
#include <string_view>

#include <boost/multiprecision/cpp_int.hpp>

namespace rdf4cpp::rdf::util {
template<typename UnscaledValue_t = boost::multiprecision::cpp_int, typename Exponent_t = uint32_t>
class BigDecimal {
    UnscaledValue_t unscaled_value;
    Exponent_t exponent;
    bool sign;

    static constexpr uint32_t base = 10;

    static constexpr UnscaledValue_t shift_pow(UnscaledValue_t v, Exponent_t ex) {
        for (Exponent_t i = 0; i < ex; ++i)
            v *= base;
        return v;
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

    constexpr std::strong_ordering operator<=>(const BigDecimal& other) const noexcept {
            if (this->sign != other.sign)
                return this->sign == true ? std::strong_ordering::less : std::strong_ordering::greater;
            UnscaledValue_t t = this->unscaled_value;
            UnscaledValue_t o = other.unscaled_value;
            if (this->exponent > other.exponent) {
                o = shift_pow(o, this->exponent - other.exponent);
            }
            else if (this->exponent < other.exponent) {
                t = shift_pow(t, other.exponent - this->exponent);
            }
            if (t < o)
                return std::strong_ordering::less;
            else if(t > o)
                return std::strong_ordering::greater;
            else
                return std::strong_ordering::equivalent;
    };
    constexpr bool operator==(const BigDecimal& other) const noexcept {
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
                    s << '.';
                    hasDot = true;
                }
                else {
                    --ex;
                }
                uint32_t c = static_cast<uint32_t>(v % base);
                s << c;
                v /= base;
        }
        if (sign)
                s << '-';
        std::string_view sv = s.view();
        return std::string {sv.rbegin(), sv.rend()};
    }
};
}

#endif  //RDF4CPP_BIGDECIMAL_H
