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

struct IStreamQuadIterator::Impl {
private:
    using PrefixMap = rdf4cpp::rdf::storage::util::tsl::sparse_map<
            std::string,
            std::string,
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::string_view>,
            std::equal_to<>>;

    struct SerdReaderDelete {
        inline void operator()(SerdReader *rdr) const noexcept {
            serd_reader_end_stream(rdr);
            serd_reader_free(rdr);
        }
    };

    using OwnedSerdReader = std::unique_ptr<SerdReader, SerdReaderDelete>;

    std::reference_wrapper<std::istream> istream;
    mutable storage::node::NodeStorage node_storage;

    OwnedSerdReader reader;

    PrefixMap prefixes;
    std::deque<Quad> quad_buffer;
    std::optional<ParsingError> last_error;
    bool end_flag = false;

private:
    static std::string_view node_into_string_view(SerdNode const *node) noexcept;
    static ParsingError::Type parsing_error_type_from_serd(SerdStatus st) noexcept;

private:
    BlankNode get_bnode(SerdNode const *node) const;
    IRI get_uri(SerdNode const *node) const;
    nonstd::expected<IRI, SerdStatus> get_prefixed_uri(SerdNode const *node) const;
    Literal get_literal(SerdNode const *literal, SerdNode const *datatype, SerdNode const *lang) const;

    static SerdStatus on_error(void *voided_self, SerdError const *error) noexcept;
    static SerdStatus on_base(void *voided_self, const SerdNode *uri) noexcept;
    static SerdStatus on_prefix(void *voided_self, SerdNode const *name, SerdNode const *uri) noexcept;
    static SerdStatus on_stmt(void *voided_self, SerdStatementFlags, SerdNode const *graph, SerdNode const *subj, SerdNode const *pred, SerdNode const *obj, SerdNode const *obj_datatype, SerdNode const *obj_lang) noexcept;

public:
    Impl(std::istream &istream, bool strict, storage::node::NodeStorage node_storage) noexcept;

    /**
     * @return true if this will no longer yield values
     * @note one sided implication, could be false and still not yield another value
     */
    [[nodiscard]] inline bool is_at_end() const noexcept {
        return this->end_flag && quad_buffer.empty();
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
    [[nodiscard]] std::optional<nonstd::expected<Quad, ParsingError>> next() noexcept;
};

}  // namespace rdf4cpp::rdf::parser

#endif  // RDF4CPP_PARSER_PRIVATE_IMPL_HPP
