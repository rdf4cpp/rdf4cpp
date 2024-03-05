#ifndef RDF4CPP_CHARMATCHER_HPP
#define RDF4CPP_CHARMATCHER_HPP

#include <string_view>

/**
 * purpose-build for IRIView::quick_validate and only included there.
 */
namespace rdf4cpp::util::char_matcher_detail {

template<class T>
concept CharMatcher = requires(const T a, int c) {{a.match(c)} -> std::same_as<bool>; };

/**
 * matches, if any char in pattern matches. does compare char by char, so no utf8.
 */
struct ASCIIPatternMatcher {
    std::string_view pattern;

    [[nodiscard]] constexpr bool match(int c) const noexcept {
        auto ch = static_cast<char>(c);
        if (c != static_cast<int>(ch))  // not asciii
            return false;
        return pattern.find(ch) != std::string_view::npos;
    }
};

/**
 * matches ascii numbers 0-9
 */
struct ASCIINumMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        auto ch = static_cast<char>(c);
        if (c != static_cast<int>(ch))  // not asciii
            return false;
        return c >= '0' && c <= '9';
    }
};
/**
 * matches ascii a-z or A-Z
 */
struct ASCIIAlphaMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        auto ch = static_cast<char>(c);
        if (c != static_cast<int>(ch))  // not asciii
            return false;
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
};

/**
 * matches if a or b matches.
 * @tparam A
 * @tparam B
 */
template<CharMatcher A, CharMatcher B>
struct OrMatcher {
    A a;
    B b;

    constexpr OrMatcher(A a, B b) : a(a), b(b) {}
    constexpr OrMatcher() = default;

    [[nodiscard]] constexpr bool match(int c) const noexcept {
        return a.match(c) || b.match(c);
    }
};

/**
 * shortcut to create a OrMatcher.
 * @tparam A
 * @tparam B
 * @param a
 * @param b
 * @return
 */
template<CharMatcher A, CharMatcher B>
constexpr OrMatcher<A, B> operator|(A a, B b) {
    return OrMatcher{a, b};
}

/**
 * matches ascii 0-9, a-z or A-Z.
 */
constexpr auto ascii_alphanum_matcher = ASCIIAlphaMatcher{} | ASCIINumMatcher{};

/**
 * matches ucschar of the IRI specification.
 */
struct UCSCharMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        return (c >= 0xA0 && c <= 0xD7FF) ||
               (c >= 0xF900 && c <= 0xFDCF) ||
               (c >= 0xFDF0 && c <= 0xFFEF) ||
               (c >= 0x10000 && c <= 0x1FFFD) ||
               (c >= 0x20000 && c <= 0x2FFFD) ||
               (c >= 0x30000 && c <= 0x3FFFD) ||
               (c >= 0x40000 && c <= 0x4FFFD) ||
               (c >= 0x50000 && c <= 0x5FFFD) ||
               (c >= 0x60000 && c <= 0x6FFFD) ||
               (c >= 0x70000 && c <= 0x7FFFD) ||
               (c >= 0x80000 && c <= 0x8FFFD) ||
               (c >= 0x90000 && c <= 0x9FFFD) ||
               (c >= 0xA0000 && c <= 0xAFFFD) ||
               (c >= 0xB0000 && c <= 0xBFFFD) ||
               (c >= 0xC0000 && c <= 0xCFFFD) ||
               (c >= 0xD0000 && c <= 0xDFFFD) ||
               (c >= 0xE0000 && c <= 0xEFFFD);
    }
};

/**
 * matches iunreserved of the IRI specification.
 */
constexpr auto i_unreserved_matcher = ascii_alphanum_matcher | ASCIIPatternMatcher{"-._~"} | UCSCharMatcher{};
/**
 * matches sub-delim of the IRI (and URI) specification.
 */
constexpr auto sub_delims_matcher = ASCIIPatternMatcher{"!$&'()*+,;="};

/**
 * matches iprivate of the IRI specification.
 */
struct IPrivateMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        return (c >= 0xE000 && c <= 0xF8FF) ||
               (c >= 0xF0000 && c <= 0xFFFFD) ||
               (c >= 0x100000 && c <= 0x10FFFD);
    }
};

/**
 * iterates over s and tries to match all in m.
 * S == std::string_view: match ascii chars.
 * S == std::string_view | una::views::utf8: match unicode codepoints.
 * @tparam M
 * @tparam S
 * @param m
 * @param s
 * @return
 */
template<CharMatcher M, class S>
bool match(const M &m, S s) noexcept {
    for (int c : s) {
        if (!m.match(c))
            return false;
    }
    return true;
}

} // namespace rdf4cpp::util::char_matcher_detail

#endif  //RDF4CPP_CHARMATCHER_HPP
