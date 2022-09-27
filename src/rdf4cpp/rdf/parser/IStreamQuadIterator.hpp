#ifndef RDF4CPP_ISTREAMQUADITERATOR_HPP
#define RDF4CPP_ISTREAMQUADITERATOR_HPP

#include <iterator>
#include <memory>

#include <nonstd/expected.hpp>
#include <rdf4cpp/rdf/Quad.hpp>

namespace rdf4cpp::rdf::parser {

/**
 * An error that might occur while parsing a turtle file
 */
struct ParsingError {
    enum struct Type {
        EofReached,
        BadSyntax,
        BadCurie,
    };

    Type error_type;
    uint32_t line;
    uint32_t col;
    std::string message;

    inline friend std::ostream &operator<<(std::ostream &os, ParsingError::Type const &error_type) noexcept {
        switch (error_type) {
            case Type::EofReached:
                os << "unexpected end of file";
                break;
            case Type::BadCurie:
                os << "bad curie";
                break;
            case Type::BadSyntax:
                os << "bad syntax";
                break;
        }

        return os;
    }

    inline friend std::ostream &operator<<(std::ostream &os, ParsingError const &error) noexcept {
        os << error.line << ':' << error.col << '(' << error.error_type << "): " << error.message;
        return os;
    }
};

enum struct ParsingFlag : uint8_t {
    Strict = 1 << 0,
    StopOnFirstError = 1 << 1,
};

struct ParsingFlags {
private:
    using flag_u_type = std::underlying_type_t<ParsingFlag>;
    flag_u_type flags;

    constexpr ParsingFlags(uint8_t const flags) : flags{flags} {}

public:
    constexpr ParsingFlags(ParsingFlag const flag)
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

/**
 * Simmilar to std::istream_iterator<>.
 * Parses the given istream and tries to extract Quads given in TURTLE format.
 *
 * @note the iterator _starts on_ the first Quad
 * @note An exhausted iterator becomes the end-of-stream iterator.
 *
 * @example
 * @code
 * std::ifstream ifs{"triples.nt"};
 *
 * for (auto qit = IStreamQuadIterator{ifs}; qit != IStreamQuadIterator{}; ++qit) {
 *      if (qit->has_value()) {
 *          std::cout << qit->value() << std::endl;
 *      } else {
 *          std::cerr << qit->error() << std::endl;
 *      }
 * }
 * @endcode
 */
struct IStreamQuadIterator {
    using value_type = nonstd::expected<Quad, ParsingError>;
    using reference = value_type const &;
    using pointer = value_type const *;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using istream_type = std::istream;

private:
    struct Impl;

    std::unique_ptr<Impl> impl;
    nonstd::expected<Quad, ParsingError> cur = nonstd::make_unexpected(ParsingError{ .error_type = ParsingError::Type::EofReached, .line = 0, .col = 0, .message = "eof reached" });

    [[nodiscard]] bool is_at_end() const;
public:
    /**
     * Constructs the end-of-stream iterator
     */
    IStreamQuadIterator() noexcept;

    /**
     * Constructs the end-of-stream iterator
     */
    IStreamQuadIterator(std::default_sentinel_t) noexcept;

    IStreamQuadIterator(IStreamQuadIterator const &) = delete;
    IStreamQuadIterator(IStreamQuadIterator &&) = default;

    explicit IStreamQuadIterator(std::istream &istream, ParsingFlags flags = ParsingFlags::none(), storage::node::NodeStorage &node_storage = storage::node::NodeStorage::default_instance());

    ~IStreamQuadIterator();

    reference operator*() const noexcept;
    pointer operator->() const noexcept;
    IStreamQuadIterator &operator++() noexcept;

    bool operator==(IStreamQuadIterator const &) const noexcept;
    bool operator!=(IStreamQuadIterator const &) const noexcept;
};

} // namespace rdf4cpp::rdf::parser

#endif  //RDF4CPP_ISTREAMQUADITERATOR_HPP
