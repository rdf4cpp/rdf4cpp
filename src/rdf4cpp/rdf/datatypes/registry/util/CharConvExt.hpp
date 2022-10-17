#ifndef RDF4CPP_REGISTRY_CHARCONVEXT_HPP
#define RDF4CPP_REGISTRY_CHARCONVEXT_HPP

#include <charconv>
#include <concepts>
#include <stdexcept>

namespace rdf4cpp::rdf::datatypes::registry::util {

template<std::integral I>
I from_chars(std::string_view s) {
    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    I value;
    auto const parse_res = std::from_chars(s.data(), s.data() + s.size(), value);

    if (parse_res.ptr != s.data() + s.size()) {
        throw std::runtime_error{"integral parsing error: " + std::make_error_code(parse_res.ec).message()};
    } else {
        return value;
    }
}

template<std::integral I>
std::string to_chars(I const value) noexcept {
    // +1 because of definition of digits10 https://en.cppreference.com/w/cpp/types/numeric_limits/digits10
    // +1 for sign
    static constexpr size_t buf_sz = std::numeric_limits<I>::digits10 + 1 + static_cast<size_t>(std::is_signed_v<I>);

    std::array<char, buf_sz> buf;
    std::to_chars_result const res = std::to_chars(buf.data(), buf.data() + buf.size(), value);

    if (res.ec != std::errc{}) {
        assert(false);
        __builtin_unreachable();
    }

    return std::string{buf.data(), res.ptr};
}

template<std::floating_point F>
F from_chars(std::string_view s) {
    if (s.starts_with('+')) {
        // from_chars does not allow initial +
        s.remove_prefix(1);
    }

    F value;
    std::from_chars_result const res = std::from_chars(s.data(), s.data() + s.size(), value, std::chars_format::general);

    if (res.ptr != s.data() + s.size()) {
        // parsing did not reach end of string => it contains invalid characters
        throw std::runtime_error{"XSD Parsing Error"};
    }

    return value;
}

template<std::floating_point F>
std::string to_chars(F const value) noexcept {
    // +2 for sign and dot
    static constexpr size_t buf_sz = 2 + std::numeric_limits<F>::max_exponent10 + std::numeric_limits<F>::max_digits10;

    std::array<char, buf_sz> buf;
    std::to_chars_result const res = std::to_chars(buf.data(), buf.data() + buf.size(), value, std::chars_format::fixed);

    if (res.ec != std::errc{}) {
        assert(false);
        __builtin_unreachable();
    }

    return std::string{buf.data(), res.ptr};
}

} // namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_CHARCONVEXT_HPP
