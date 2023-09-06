#ifndef RDF4CPP_CHECKEDINT_HPP
#define RDF4CPP_CHECKEDINT_HPP

#endif  //RDF4CPP_CHECKEDINT_HPP

namespace rdf4cpp::rdf::util {
template<std::integral I>
class CheckedIntegral {
    I value;
    bool invalid = false;

public:
    constexpr CheckedIntegral(I value, bool invalid = false) noexcept : value(value), invalid(invalid) {}

    [[nodiscard]]constexpr  bool is_invalid() const noexcept {
        return invalid;
    }
    [[nodiscard]]constexpr  I get_value() const noexcept {
        return value;
    }

    constexpr std::partial_ordering operator<=>(const CheckedIntegral &other) const noexcept {
        if (this->invalid && other.invalid)
            return std::partial_ordering::equivalent;
        if (this->invalid != other.invalid)
            return std::partial_ordering::unordered;
        return this->value <=> other.value;
    }

    constexpr CheckedIntegral &operator+=(const CheckedIntegral &other) noexcept {
        if (this->invalid || other.invalid) {
            this->invalid = true;
            return *this;
        }
        if (__builtin_add_overflow(this->value, other.value, &this->value)) {
            this->invalid = true;
            return *this;
        }
        return *this;
    }
    constexpr CheckedIntegral operator+(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r += other;
        return r;
    }
    constexpr CheckedIntegral &operator-=(const CheckedIntegral &other) noexcept {
        if (this->invalid || other.invalid) {
            this->invalid = true;
            return *this;
        }
        if (__builtin_sub_overflow(this->value, other.value, &this->value)) {
            this->invalid = true;
            return *this;
        }
        return *this;
    }
    constexpr CheckedIntegral operator-(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r -= other;
        return r;
    }
    constexpr CheckedIntegral &operator*=(const CheckedIntegral &other) noexcept {
        if (this->invalid || other.invalid) {
            this->invalid = true;
            return *this;
        }
        if (__builtin_mul_overflow(this->value, other.value, &this->value)) {
            this->invalid = true;
            return *this;
        }
        return *this;
    }
    constexpr CheckedIntegral operator*(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r *= other;
        return r;
    }
    constexpr CheckedIntegral &operator/=(const CheckedIntegral &other) noexcept {
        if (this->invalid || other.invalid || other.value == 0) {
            this->invalid = true;
            return *this;
        }
        this->value = this->value / other.value;
        return *this;
    }
    constexpr CheckedIntegral operator/(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r /= other;
        return r;
    }
};

}  // namespace rdf4cpp::rdf::util