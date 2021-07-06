#ifndef RDF4CPP_FLOAT_HPP
#define RDF4CPP_FLOAT_HPP


#include <ostream>
#include <rdf4cpp/rdf/datatypes/DatatypeRegistry.hpp>

namespace rdf4cpp::rdf::datatypes::xsd {

/**
 * Implements <a href="http://www.w3.org/2001/XMLSchema#float">xsd:float</a>
 */
class Float : DatatypeBase<Float> {

    float value_{};

public:
    static constexpr char datatype_iri[] = "http://www.w3.org/2001/XMLSchema#float";

    Float() = default;
    explicit Float(const std::string &str)
        // TODO: that might not be completely compliant with xsd yet
        : value_(std::stof(str)) {}


    Float(float value) noexcept : value_(value) {}

    Float &operator=(float value) noexcept {
        value_ = value;
        return *this;
    }
    Float &operator=(char value) noexcept {
        value_ = value;
        return *this;
    }
    Float &operator=(short value) noexcept {
        value_ = value;
        return *this;
    }

    explicit operator float() const noexcept {
        return value_;
    }

    Float &operator+=(Float rhs) noexcept {
        value_ += rhs.value_;
        return *this;
    }
    Float &operator-=(Float rhs) noexcept {
        value_ -= rhs.value_;
        return *this;
    }
    Float &operator*=(Float rhs) noexcept {
        value_ *= rhs.value_;
        return *this;
    }
    Float &operator/=(Float rhs) {
        value_ /= rhs.value_;
        return *this;
    }

    friend Float operator+(Float lhs, Float rhs) noexcept {
        return lhs.value_ + rhs.value_;
    }
    friend Float operator-(Float lhs, Float rhs) noexcept {
        return lhs.value_ - rhs.value_;
    }
    friend Float operator*(Float lhs, Float rhs) noexcept {
        return lhs.value_ * rhs.value_;
    }
    friend Float operator/(Float lhs, Float rhs) {
        return lhs.value_ / rhs.value_;
    }

    friend std::ostream &operator<<(std::ostream &os, const Float &inst) {
        os << inst.value_;
        return os;
    }

    explicit operator std::string() {
        // TODO: this might not be xsd canonical syntax
        return std::to_string(value_);
    }


    friend bool operator<(const Float &lhs, const Float &rhs) {
        return lhs.value_ < rhs.value_;
    }
    friend bool operator>(const Float &lhs, const Float &rhs) {
        return lhs.value_ > rhs.value_;
    }
    friend bool operator<=(const Float &lhs, const Float &rhs) {
        return lhs.value_ <= rhs.value_;
    }
    friend bool operator>=(const Float &lhs, const Float &rhs) {
        return lhs.value_ >= rhs.value_;
    }
    friend bool operator==(const Float &lhs, const Float &rhs) {
        return lhs.value_ == rhs.value_;
    }
    friend bool operator!=(const Float &lhs, const Float &rhs) {
        return lhs.value_ == rhs.value_;
    }
};
}  // namespace rdf4cpp::rdf::datatypes::xsd

#endif  //RDF4CPP_FLOAT_HPP
