#ifndef RDF4CPP_PARSER_ISTREAMQUADITERATOR_HPP
#define RDF4CPP_PARSER_ISTREAMQUADITERATOR_HPP

#include <iterator>
#include <memory>

#include <nonstd/expected.hpp>

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/parser/ParsingError.hpp>
#include <rdf4cpp/rdf/parser/ParsingFlags.hpp>
#include <rdf4cpp/rdf/parser/ParsingState.hpp>

namespace rdf4cpp::rdf::parser {

/**
 * Similar to std::istream_iterator<>.
 * Parses the given istream and tries to extract Quads given in TURTLE format.
 *
 * @note the iterator _starts on_ the first Quad
 * @note An exhausted iterator becomes the end-of-stream iterator.
 * @warning The value pointed to by an end-of-stream iterator is undefined
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
    using flags_type = ParsingFlags;
    using state_type = ParsingState;
    using ok_type = Quad;
    using error_type = ParsingError;

    using value_type = nonstd::expected<ok_type, error_type>;
    using reference = value_type const &;
    using pointer = value_type const *;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;
    using istream_type = std::istream;

private:
    struct Impl;

    std::unique_ptr<Impl> impl;
    nonstd::expected<Quad, error_type> cur = nonstd::make_unexpected(ParsingError{.error_type = ParsingError::Type::EofReached, .line = 0, .col = 0, .message = "eof reached"});

    [[nodiscard]] bool is_at_end() const noexcept;

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
    IStreamQuadIterator(IStreamQuadIterator &&) noexcept;

    /**
     * Constructs an IStreamQuad iterator to parse an input stream in turtle syntax to quads
     *
     * @param istream turtle input stream
     * @param flags specifies the parser behaviour
     * @param initial_state optionally specifies the initial state of the parser,
     *          providing nullptr as the initial state results in the parser creating its own,fresh state
     *          instead of writing to the provided state.
     */
    explicit IStreamQuadIterator(istream_type &istream,
                                 flags_type flags = flags_type::none(),
                                 state_type *initial_state = nullptr) noexcept;
    ~IStreamQuadIterator() noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;
    IStreamQuadIterator &operator++();

    bool operator==(IStreamQuadIterator const &) const noexcept;
    bool operator!=(IStreamQuadIterator const &) const noexcept;
};

}  // namespace rdf4cpp::rdf::parser

#endif  //RDF4CPP_PARSER_ISTREAMQUADITERATOR_HPP
