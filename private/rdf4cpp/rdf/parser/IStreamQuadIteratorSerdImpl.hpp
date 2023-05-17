#ifndef RDF4CPP_PARSER_PRIVATE_IMPL_HPP
#define RDF4CPP_PARSER_PRIVATE_IMPL_HPP

#include <deque>
#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <serd/serd.h>

#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/parser/IStreamQuadIterator.hpp>
#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>

namespace rdf4cpp::rdf::parser {

struct IStreamQuadIterator::Impl {
private:
    using PrefixMap = IStreamQuadIterator::prefix_storage_type;

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
    bool no_parse_prefixes;

private:
    static std::string_view node_into_string_view(SerdNode const *node) noexcept;
    static ParsingError::Type parsing_error_type_from_serd(SerdStatus st) noexcept;

private:
    nonstd::expected<BlankNode, SerdStatus> get_bnode(SerdNode const *node) noexcept;
    nonstd::expected<IRI, SerdStatus> get_iri(SerdNode const *node) noexcept;
    nonstd::expected<IRI, SerdStatus> get_prefixed_iri(SerdNode const *node) noexcept;
    nonstd::expected<Literal, SerdStatus> get_literal(SerdNode const *literal, SerdNode const *datatype, SerdNode const *lang) noexcept;

    static SerdStatus on_error(void *voided_self, SerdError const *error) noexcept;
    static SerdStatus on_base(void *voided_self, SerdNode const *uri) noexcept;
    static SerdStatus on_prefix(void *voided_self, SerdNode const *name, SerdNode const *uri) noexcept;
    static SerdStatus on_stmt(void *voided_self, SerdStatementFlags, SerdNode const *graph, SerdNode const *subj, SerdNode const *pred, SerdNode const *obj, SerdNode const *obj_datatype, SerdNode const *obj_lang) noexcept;

    static constexpr SerdSyntax extract_syntax_from_flags(ParsingFlags flags) noexcept {
        switch (flags.get_syntax()) {
            case ParsingFlag::TriG:
                return SerdSyntax::SERD_TRIG;
            case ParsingFlag::NTriples:
                return SerdSyntax::SERD_NTRIPLES;
            case ParsingFlag::NQuads:
                return SerdSyntax::SERD_NQUADS;
            default:
                return SerdSyntax::SERD_TURTLE;
        }
    }

public:
    Impl(std::istream &istream, ParsingFlags flags, PrefixMap prefixes, storage::node::NodeStorage node_storage) noexcept;

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
     * Will try to skip over errors so that the next call might be able to return a value.
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
