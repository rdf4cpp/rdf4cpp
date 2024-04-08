#ifndef RDF4CPP_CHARMATCHER_HPP
#define RDF4CPP_CHARMATCHER_HPP

#include <array>
#include <optional>
#include <string_view>

#include <rdf4cpp/datatypes/registry/util/ConstexprString.hpp>

/**
 * purpose-build for IRIView::quick_validate and only included there.
 */
namespace rdf4cpp::util::char_matcher_detail {
struct CharRange {
    char first = '\0';
    char last = '\0';
};

template<typename T>
concept CharMatcher = requires(T const a, int c) {
    {
        a.match(c)
    } -> std::same_as<bool>;
    {
        T::SIMD_RANGE_NUM
    } -> std::same_as<const size_t &>;
    {
        T::FAIL_IF_UNICODE
    } -> std::same_as<const bool &>;
    {
        a.simd_ranges()
    } -> std::same_as<std::array<CharRange, T::SIMD_RANGE_NUM>>;
    {
        a.simd_singles()
    };  // -> std::same_as<std::string_view>;
};

static constexpr size_t simd_max_single_chars = 32;
std::optional<bool> try_match_simd(std::string_view data, std::array<CharRange, 3> const &ranges, std::string_view single);

/**
 * matches, if any char in pattern matches. does compare char by char, so no utf8.
 */
template<size_t n>
struct ASCIIPatternMatcher {
    datatypes::registry::util::ConstexprString<n> pattern;

    explicit constexpr ASCIIPatternMatcher(char const (&str)[n]) noexcept
        : pattern(str) {
    }

    [[nodiscard]] constexpr bool match(int c) const noexcept {
    auto ch = static_cast<char>(c);
    if (c != static_cast<int>(ch))  // not asciii
        return false;
    return static_cast<std::string_view>(pattern).find(ch) != std::string_view::npos;
    }

    static constexpr size_t SIMD_RANGE_NUM = 0;
    static constexpr bool FAIL_IF_UNICODE = true;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return {};
    }
    [[nodiscard]] consteval auto simd_singles() const noexcept {
        return pattern;
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

    static constexpr size_t SIMD_RANGE_NUM = 1;
    static constexpr bool FAIL_IF_UNICODE = true;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return std::array<CharRange, SIMD_RANGE_NUM>{CharRange{'0', '9'}};
    }
    [[nodiscard]] static consteval auto simd_singles() noexcept {
        return datatypes::registry::util::ConstexprString("");
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

    static constexpr size_t SIMD_RANGE_NUM = 2;
    static constexpr bool FAIL_IF_UNICODE = true;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return std::array<CharRange, SIMD_RANGE_NUM>{
                CharRange{'a', 'z'},
                CharRange{'A', 'Z'}};
    }
    [[nodiscard]] static consteval auto simd_singles() noexcept {
        return datatypes::registry::util::ConstexprString("");
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

    static constexpr size_t SIMD_RANGE_NUM = A::SIMD_RANGE_NUM + B::SIMD_RANGE_NUM;
    static constexpr bool FAIL_IF_UNICODE = A::FAIL_IF_UNICODE && B::FAIL_IF_UNICODE;
    [[nodiscard]] consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() const noexcept {
        std::array<CharRange, SIMD_RANGE_NUM> r{};
        if constexpr (A::SIMD_RANGE_NUM > 0) {
            auto aa = a.simd_ranges();
            for (size_t s = 0; s < A::SIMD_RANGE_NUM; ++s) {
                r[s] = aa[s];
            }
        }
        if constexpr (B::SIMD_RANGE_NUM > 0) {
            auto ba = b.simd_ranges();
            for (size_t s = 0; s < B::SIMD_RANGE_NUM; ++s) {
                r[s + A::SIMD_RANGE_NUM] = ba[s];
            }
        }
        return r;
    }
    [[nodiscard]] consteval auto simd_singles() const noexcept {
        return a.simd_singles() + b.simd_singles();
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

    static constexpr size_t SIMD_RANGE_NUM = 0;
    static constexpr bool FAIL_IF_UNICODE = false;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return {};
    }
    [[nodiscard]] static consteval auto simd_singles() noexcept {
        return datatypes::registry::util::ConstexprString("");
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

    static constexpr size_t SIMD_RANGE_NUM = 0;
    static constexpr bool FAIL_IF_UNICODE = false;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return {};
    }
    [[nodiscard]] static consteval auto simd_singles() noexcept {
        return datatypes::registry::util::ConstexprString("");
    }
};

/**
 * matches PN_CHARS_BASE of the Turtle/SPARQL specification, without ASCII alpha (or with ASCIIAlphaMatcher).
 */
struct PNCharsBase_UniMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        return  c == '_' ||
               (c >= 0xC0 && c <= 0xD6) ||
               (c >= 0xD8 && c <= 0xF6) ||
               (c >= 0xF8 && c <= 0x02FF) ||
               (c >= 0x0370 && c <= 0x037D) ||
               (c >= 0x037F && c <= 0x1FFF) ||
               (c >= 0x200C && c <= 0x200D) ||
               (c >= 0x2070 && c <= 0x218F) ||
               (c >= 0x2C00 && c <= 0x2FEF) ||
               (c >= 0x3001 && c <= 0xD7FF) ||
               (c >= 0xF900 && c <= 0xFDCF) ||
               (c >= 0xFDF0 && c <= 0xFFFD) ||
               (c >= 0x00010000 && c <= 0x000EFFFF);
    }

    static constexpr size_t SIMD_RANGE_NUM = 0;
    static constexpr bool FAIL_IF_UNICODE = false;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return {};
    }
    [[nodiscard]] static consteval auto simd_singles() noexcept {
        return datatypes::registry::util::ConstexprString("");
    }
};

/**
 * matches PN_CHARS_BASE of the Turtle/SPARQL specification
 */
constexpr auto PNCharsBaseMatcher = ASCIIAlphaMatcher{} | PNCharsBase_UniMatcher{};

/**
 * matches PN_CHARS of the Turtle/SPARQL specification, without ASCII num and PN_CHARS_BASE.
 */
struct PNChars_UniMatcher {
    [[nodiscard]] static constexpr bool match(int c) noexcept {
        return c == 0xB7 ||
               (c >= 0x0300 && c <= 0x036F) ||
               (c >= 0x203F && c <= 0x2040);
    }

    static constexpr size_t SIMD_RANGE_NUM = 0;
    static constexpr bool FAIL_IF_UNICODE = false;
    [[nodiscard]] static consteval std::array<CharRange, SIMD_RANGE_NUM> simd_ranges() noexcept {
        return {};
    }
    [[nodiscard]] static consteval auto simd_singles() noexcept {
        return datatypes::registry::util::ConstexprString("");
    }
};

/**
 * matches PN_CHARS of the Turtle/SPARQL specification.
 */
constexpr auto PNCharsMatcher = ASCIINumMatcher{} | PNCharsBaseMatcher | PNChars_UniMatcher{};

/**
  * iterates over s and tries to match all in m.
  * @tparam m a CharMatcher
  * @tparam utf8_range_decoder needs to be una::views::utf8 (if i include it here, una technically becomes part of our public API)
  * @param s
  * @return
  */
template<auto const &m, auto utf8_range_decoder>
bool match(std::string_view s) noexcept {
    auto ra = m.simd_ranges();
    std::array<CharRange, 3> ranges{};
    for (size_t i = 0; i < std::min(ra.size(), ranges.size()); ++i) {
        ranges[i] = ra[i];
    }
    static constexpr auto singles = m.simd_singles();
    auto simd_r = try_match_simd(s, ranges, singles);
    if (simd_r.has_value())
        return *simd_r;
    if (m.FAIL_IF_UNICODE)
        return false;
    for (int c : s | utf8_range_decoder) {
        if (!m.match(c))
            return false;
    }
    return true;
}
} // namespace rdf4cpp::util::char_matcher_detail

#endif  //RDF4CPP_CHARMATCHER_HPP
