#ifndef RDF4CPP_PARSER_ISTREAMQUADITERATOR_HPP
#define RDF4CPP_PARSER_ISTREAMQUADITERATOR_HPP

#include <iterator>
#include <memory>

#include <rdf4cpp/Expected.hpp>

#include <rdf4cpp/Quad.hpp>

#include <rdf4cpp/parser/ParsingError.hpp>
#include <rdf4cpp/parser/ParsingFlags.hpp>
#include <rdf4cpp/parser/ParsingState.hpp>
#include <rdf4cpp/IRIFactory.hpp>

namespace rdf4cpp::parser {

/**
 * Identical semantics to fread.
 * Uses stream to read at most count elements of size element_size into buffer.
 *
 * @param buffer pointer to buffer with at least count elements of size elem_size
 * @param elem_size sizeof each element
 * @param count number of elements to read
 * @param stream pointer to any object.
 * @return number of elements read
 */
using ReadFunc = size_t (*)(void *buffer, size_t elem_size, size_t count, void *stream);

/**
 * Identical semantics to ferror.
 *
 * @param stream pointer to any object
 * @return nonzero value if there is an error in stream, zero value otherwise
 */
using ErrorFunc = int (*)(void *stream);

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
    std::optional<nonstd::expected<ok_type, error_type>> cur;

public:
    /**
     * Constructs a IStreamQuadIterator from a C-like io api. That is something similar to
     * the triple (FILE *, fread, ferror) (parameters are called (stream, read, error) here).
     *
     * @param stream Pointer to a voided object, could be a FILE * cast to void * or anything else.
     * @param read see ReadFunc docs, elem_size is guaranteed to be always set to 1
     * @param error see ErrorFunc docs
     * @param flags flags to pass to the parser
     * @param initial_state optionally specifies the initial state of the parser,
     *          providing nullptr as the initial state results in the parser creating its own,fresh state
     *          instead of writing to the provided state.
     */
    IStreamQuadIterator(void *stream,
                        ReadFunc read,
                        ErrorFunc error,
                        flags_type flags = ParsingFlags::none(),
                        state_type *initial_state = nullptr);

    /**
     * Constructs an IStreamQuadIterator to parse an input stream in turtle syntax to quads
     *
     * @param istream turtle input stream
     * @param flags specifies the parser behaviour
     * @param initial_state optionally specifies the initial state of the parser,
     *          providing nullptr as the initial state results in the parser creating its own,fresh state
     *          instead of writing to the provided state.
     */
    explicit IStreamQuadIterator(std::istream &istream,
                                 flags_type flags = ParsingFlags::none(),
                                 state_type *initial_state = nullptr);

    IStreamQuadIterator(IStreamQuadIterator const &) = delete;
    IStreamQuadIterator(IStreamQuadIterator &&) noexcept;

    IStreamQuadIterator &operator=(IStreamQuadIterator const &) = delete;
    IStreamQuadIterator &operator=(IStreamQuadIterator &&) noexcept;

    ~IStreamQuadIterator() noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;
    IStreamQuadIterator &operator++();

    [[nodiscard]] uint64_t current_line() const noexcept;
    [[nodiscard]] uint64_t current_column() const noexcept;

    bool operator==(std::default_sentinel_t) const noexcept;
    bool operator!=(std::default_sentinel_t) const noexcept;
};

/**
 * Opens a file for fast sequential access.
 * Otherwise behaves the same as fopen.
 * Inspired by serd_fopen.
 */
FILE *fopen_fastseq(char const *path, char const *mode) noexcept;

}  // namespace rdf4cpp::parser

#endif  //RDF4CPP_PARSER_ISTREAMQUADITERATOR_HPP
