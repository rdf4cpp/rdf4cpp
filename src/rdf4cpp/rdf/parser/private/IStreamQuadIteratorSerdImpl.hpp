#ifndef RDF4CPP_PARSER_PRIVATE_IMPL_HPP
#define RDF4CPP_PARSER_PRIVATE_IMPL_HPP

#include <rdf4cpp/rdf/parser/IStreamQuadIterator.hpp>

#include <cassert>
#include <cstddef>
#include <deque>
#include <istream>
#include <string>
#include <utility>

#if __has_include(<serd/serd.h>)
#include <serd/serd.h>
#else
#include <serd-0/serd/serd.h>
#endif

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>

namespace rdf4cpp::rdf::parser {

namespace util {

/**
 * A wrapper type around std::istream which is
 * intended to be used as a serd source.
 * Will keep track of the current line and column.
 *
 * @example
 * @code
 * SerdReader *reader = serd_reader_new(...);
 * std::istringstream iss{"<hello> <world> \"1\" ."};
 * IStreamAdaptor adaptor{std::ref(iss)};
 *
 * serd_reader_start_source_stream(reader, &IStreamAdaptor::read<true>, &IStreamAdaptor::is_ok, &adaptor, nullptr, 1);
 * @endcode
 */
struct IStreamAdaptor {
    std::reference_wrapper<std::istream> istream;
    uint64_t line_off = 0;
    uint64_t col_off = 0;

    static size_t read(void *buf, [[maybe_unused]] size_t elem_size, [[maybe_unused]] size_t count, void *voided_self) {
        assert(elem_size == 1);
        assert(count == 1);

        auto *self = reinterpret_cast<IStreamAdaptor *>(voided_self);
        self->istream.get().read(reinterpret_cast<char *>(buf), 1);

        auto const bytes_read = self->istream.get().gcount();
        if (bytes_read > 0) {
            // TODO: remove when serd makes line and col 64bit uints; then: instead only update when an error is encountered and not end_at_first_error
            if (static_cast<char const *>(buf)[0] == '\n') {
                self->line_off += 1;
                self->col_off = 0;
            } else {
                self->col_off += 1;
            }
        }

        return bytes_read;
    }

    inline static int is_ok(void *voided_self) noexcept {
        auto *self = reinterpret_cast<IStreamAdaptor *>(voided_self);
        return self->istream.get() ? 0 : 1;
    }
};
}  // namespace util

struct IStreamQuadIterator::Impl {
private:
    using PrefixMap = rdf4cpp::rdf::storage::util::tsl::sparse_map<
            std::string,
            std::string,
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>,
            std::equal_to<>>;

    util::IStreamAdaptor istream;
    std::reference_wrapper<storage::node::NodeStorage> node_storage;
    std::unique_ptr<SerdReader, void (*)(SerdReader *)> reader;
    bool end_at_first_error;

    PrefixMap prefixes;
    std::deque<Quad> quad_buffer;
    ParsingError last_error;
    bool end_flag = false;

private:
    static std::string_view node_into_string_view(SerdNode const *node) noexcept;
    static ParsingError::Type parsing_error_type_from_serd(SerdStatus st) noexcept;
    static void destroy_serd_reader(SerdReader *reader) noexcept;

private:
    BlankNode get_bnode(SerdNode const *node);
    IRI get_uri(SerdNode const *node);
    nonstd::expected<IRI, SerdStatus> get_prefixed_uri(SerdNode const *node) const;
    Literal get_literal(SerdNode const *literal, SerdNode const *datatype, SerdNode const *lang);

    static SerdStatus on_error(void *voided_self, SerdError const *error) noexcept;
    static SerdStatus on_base(void *voided_self, const SerdNode *uri) noexcept;
    static SerdStatus on_prefix(void *voided_self, SerdNode const *name, SerdNode const *uri) noexcept;
    static SerdStatus on_stmt(void *voided_self, SerdStatementFlags, SerdNode const *graph, SerdNode const *subj, SerdNode const *pred, SerdNode const *obj, SerdNode const *obj_datatype, SerdNode const *obj_lang);

private:
    void start_stream() noexcept;
    void try_skip_error();

public:
    Impl(std::istream &istream, bool strict, bool end_at_first_error, storage::node::NodeStorage &node_storage) noexcept;

    /**
     * @return if the next call to `next` would return std::nullopt
     */
    [[nodiscard]] inline bool is_at_end() const noexcept {
        return this->end_flag || (this->istream.istream.get().eof() && this->quad_buffer.empty());
    }

    inline bool operator==(Impl const &other) const noexcept {
        return this->reader == other.reader;
    }

    /**
     * Tries to extract the next element from the serd backend.
     * If end_at_first_error is not set, will try to skip over the error
     * so that the next call might be able to return a value.
     *
     * @note Call until std::nullopt is returned
     * @return
     *      std::nullopt: if there is no next element (eof)
     *      expected Quad: if there was a next element and it could be parsed
     *      unexpected ParsingError: if there was a next element but it could not be parsed
     */
    [[nodiscard]] std::optional<nonstd::expected<Quad, ParsingError>> next();
};

}  // namespace rdf4cpp::rdf::parser

#endif  // RDF4CPP_PARSER_PRIVATE_IMPL_HPP
