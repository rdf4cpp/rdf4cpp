#ifndef RDF4CPP_CHECKEDINT_HPP
#define RDF4CPP_CHECKEDINT_HPP

#endif  //RDF4CPP_CHECKEDINT_HPP

namespace rdf4cpp::rdf::util {
template<std::integral I>
class CheckedIntegral {
    I value;
    bool invalid = false;

public:
    CheckedIntegral(I value, bool invalid = false) noexcept : value(value), invalid(invalid) {}

    [[nodiscard]] bool is_invalid() const noexcept {
        return invalid;
    }
    [[nodiscard]] I get_value() const noexcept {
        return value;
    }

    std::partial_ordering operator<=>(const CheckedIntegral &other) const noexcept {
        if (this->invalid && other.invalid)
            return std::partial_ordering::equivalent;
        if (this->invalid != other.invalid)
            return std::partial_ordering::unordered;
        return this->value <=> other.value;
    }

    CheckedIntegral &operator+=(const CheckedIntegral &other) noexcept {
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
    CheckedIntegral operator+(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r += other;
        return r;
    }
    CheckedIntegral &operator-=(const CheckedIntegral &other) noexcept {
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
    CheckedIntegral operator-(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r -= other;
        return r;
    }
    CheckedIntegral &operator*=(const CheckedIntegral &other) noexcept {
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
    CheckedIntegral operator*(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r *= other;
        return r;
    }
    CheckedIntegral &operator/=(const CheckedIntegral &other) noexcept {
        if (this->invalid || other.invalid || other.value == 0) {
            this->invalid = true;
            return *this;
        }
        this->value = this->value / other.value;
        return *this;
    }
    CheckedIntegral operator/(const CheckedIntegral &other) const noexcept {
        CheckedIntegral r = *this;
        r /= other;
        return r;
    }
};

}  // namespace rdf4cpp::rdf::util