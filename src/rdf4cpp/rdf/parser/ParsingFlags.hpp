#ifndef RDF4CPP_PARSER_PARSINGFLAGS_HPP
#define RDF4CPP_PARSER_PARSINGFLAGS_HPP

#include <cstdint>
#include <type_traits>

namespace rdf4cpp::rdf::parser {

enum struct ParsingFlag : uint8_t {
    Strict = 1 << 0,
    NoParsePrefix = 1 << 1,
    Turtle = 1 << 2,
    NTriples = 1 << 3,
    NQuads = 1 << 4,
    TriG = 1 << 5,
};

struct ParsingFlags {
private:
    using flag_u_type = std::underlying_type_t<ParsingFlag>;
    flag_u_type flags;

    constexpr ParsingFlags(uint8_t const flags) noexcept : flags{flags} {}

public:
    constexpr ParsingFlags(ParsingFlag const flag) noexcept
        : flags{static_cast<flag_u_type>(flag)} {
    }

    static constexpr ParsingFlags none() noexcept {
        return ParsingFlags{0};
    }

    [[nodiscard]] constexpr bool contains(ParsingFlag const flag) const noexcept {
        return flags & static_cast<flag_u_type>(flag);
    }

    constexpr ParsingFlags &operator|=(ParsingFlags const other) noexcept {
        this->flags |= other.flags;
        return *this;
    }

    constexpr ParsingFlags operator|(ParsingFlags const other) const noexcept {
        auto cpy = *this;
        return (cpy |= other);
    }

    constexpr ParsingFlags &operator|=(ParsingFlag const flag) noexcept {
        this->flags |= static_cast<flag_u_type>(flag);
        return *this;
    }

    constexpr ParsingFlags operator|(ParsingFlag const flag) const noexcept {
        auto cpy = *this;
        return (cpy |= flag);
    }
};

constexpr ParsingFlags operator|(ParsingFlag const f1, ParsingFlag const f2) noexcept {
    return ParsingFlags::none() | f1 | f2;
}

} // namespace rdf4cpp::rdf::parser

#endif  //RDF4CPP_PARSER_PARSINGFLAGS_HPP
