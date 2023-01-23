#ifndef RDF4CPP_REGISTRY_UTIL_LANGTAG_HPP
#define RDF4CPP_REGISTRY_UTIL_LANGTAG_HPP

#include <string_view>

namespace rdf4cpp::rdf::datatypes::registry::util {

struct LangTagCharTraits : std::char_traits<char> {
    using char_type = std::char_traits<char>::char_type;
    using pos_type = std::char_traits<char>::pos_type;
    using int_type = std::char_traits<char>::int_type;
    using off_type = std::char_traits<char>::off_type;
    using state_type = std::char_traits<char>::state_type;
    using comparison_category = std::char_traits<char>::comparison_category;

    static constexpr bool eq(char_type const c1, char_type const c2) noexcept {
        return std::tolower(c1) == std::tolower(c2);
    }

    static constexpr bool ne(char_type const c1, char_type const c2) noexcept {
        return std::tolower(c1) != std::tolower(c2);
    }

    static constexpr bool lt(char_type const c1, char_type const c2) noexcept {
        return std::tolower(c1) < std::tolower(c2);
    }

    static constexpr int compare(char_type const *s1, char_type const *s2, size_t const len) noexcept {
        for (size_t ix = 0; ix < len; ++ix) {
            auto const c1 = std::tolower(s1[ix]);
            auto const c2 = std::tolower(s2[ix]);

            if (c1 < c2) {
                return -1;
            }

            if (c1 > c2) {
                return 1;
            }
        }

        return 0;
    }

    static constexpr char_type const *find(char_type const *s, size_t const len, char const needle) noexcept {
        auto const ineedle = std::tolower(needle);

        for (size_t ix = 0; ix < len; ++ix) {
            if (std::tolower(s[ix]) == ineedle) {
                return &s[ix];
            }
        }

        return nullptr;
    }
};

using LangTagView = std::basic_string_view<char, LangTagCharTraits>;

}  //namespace rdf4cpp::rdf::datatypes::registry::util

#endif  //RDF4CPP_REGISTRY_UTIL_LANGTAG_HPP
