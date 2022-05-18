#include "rdf4cpp/rdf/Dataset.hpp"

#include <serd-0/serd/serd.h>

#include <rdf4cpp/rdf/storage/util/robin-hood-hashing/robin_hood_hash.hpp>
#include <rdf4cpp/rdf/storage/util/tsl/sparse_map.h>

using Quad = rdf4cpp::rdf::Quad;
using namespace rdf4cpp::rdf;

struct SerdHandle {
    using PrefixMap = rdf4cpp::rdf::storage::util::tsl::sparse_map<
            std::string,
            std::string,
            rdf4cpp::rdf::storage::util::robin_hood::hash<std::string>>;

    PrefixMap prefixes;
    Dataset *datset;
};

SerdStatus on_base(SerdHandle *handle, const SerdNode *uri) {
    handle->prefixes[""] = std::string((char *) (uri->buf), uri->n_bytes);
    return SERD_SUCCESS;
}

SerdStatus on_prefix(SerdHandle *handle, const SerdNode *name, const SerdNode *uri) {
    handle->prefixes[std::string((char *) (name->buf), name->n_bytes)] = std::string((char *) (uri->buf), uri->n_bytes);
    return SERD_SUCCESS;
}

BlankNode getBNode(const SerdNode *node) {
    auto identifier = std::string(std::string_view{(char *) (node->buf), size_t(node->n_bytes)});
    return BlankNode(identifier);
}

IRI getURI(const SerdNode *node) {
    auto iri = std::string(std::string_view{(char *) (node->buf), size_t(node->n_bytes)});
    return IRI(iri);
}

IRI getPrefixedUri(SerdHandle &handle, const SerdNode *node) {
    std::string_view uri_node_view{(char *) (node->buf), size_t(node->n_bytes)};
    auto sep_pos = uri_node_view.find(':');
    std::string_view prefix{uri_node_view.begin(), sep_pos};
    std::string_view suffix{uri_node_view.begin() + (sep_pos + 1), uri_node_view.size() - sep_pos - 1};
    // TODO that is not safe!
    const std::string &long_prefix = handle.prefixes[std::string{prefix}];
    std::string full_string = long_prefix + std::string{suffix};
    return IRI(full_string);
}

Literal getLiteral(const SerdNode *literal, const SerdNode *type_node, const SerdNode *lang_node) {
    std::string literal_value = std::string{(char *) (literal->buf), size_t(literal->n_bytes)};
    if (type_node != nullptr)
        return Literal(literal_value,
                       IRI(std::string{(char *) (type_node->buf), size_t(type_node->n_bytes)}));
    else if (lang_node != nullptr)
        return Literal(literal_value, std::string{(char *) (lang_node->buf), size_t(lang_node->n_bytes)});
    else
        return Literal(literal_value);
};

SerdStatus on_statement(SerdHandle *handle,
                        [[maybe_unused]] SerdStatementFlags flags,
                        const SerdNode *graph,
                        const SerdNode *subject,
                        const SerdNode *predicate,
                        const SerdNode *object,
                        const SerdNode *object_datatype,
                        const SerdNode *object_lang) {
    Quad quad;
    if (graph != nullptr)
        switch (graph->type) {
            case SERD_CURIE:
                quad.graph() = getPrefixedUri(*handle, subject);
                break;
            case SERD_URI:
                quad.graph() = getURI(subject);
                break;
            case SERD_BLANK: {
                quad.graph() = getBNode(subject);
            } break;
            default:
                return SERD_ERR_BAD_SYNTAX;
        }
    else
        quad.graph() = IRI::default_graph();

    switch (subject->type) {
        case SERD_CURIE:
            quad.subject() = getPrefixedUri(*handle, subject);
            break;
        case SERD_URI:
            quad.subject() = getURI(subject);
            break;
        case SERD_BLANK: {
            quad.subject() = getBNode(subject);
        } break;
        default:
            return SERD_ERR_BAD_SYNTAX;
    }

    switch (predicate->type) {
        case SERD_CURIE:
            quad.predicate() = getPrefixedUri(*handle, predicate);
            break;
        case SERD_URI:
            quad.predicate() = getURI(predicate);
            break;
        default:
            return SERD_ERR_BAD_SYNTAX;
    }

    switch (object->type) {
        case SERD_CURIE:
            quad.object() = getPrefixedUri(*handle, object);
            break;
        case SERD_LITERAL:
            quad.object() = getLiteral(object, object_datatype, object_lang);
            break;
        case SERD_BLANK:
            quad.object() = getBNode(object);
            break;
        case SERD_URI:
            quad.object() = getURI(object);
            break;
        default:
            return SERD_ERR_BAD_SYNTAX;
    }
    handle->datset->add(quad);
    return SERD_SUCCESS;
}

SerdStatus on_end([[maybe_unused]] SerdHandle *handle, [[maybe_unused]] const SerdNode *node) {
    return SERD_SUCCESS;
}
namespace rdf4cpp::rdf {
void import_rdf_file(const std::string &path, Dataset *dataset) {
    SerdHandle serd_handle;
    serd_handle.datset = dataset;
    SerdReader *reader = serd_reader_new(SERD_TURTLE, (void *) &serd_handle,
                                         nullptr,
                                         reinterpret_cast<SerdBaseSink>(on_base),
                                         reinterpret_cast<SerdPrefixSink>(on_prefix),
                                         reinterpret_cast<SerdStatementSink>(on_statement),
                                         reinterpret_cast<SerdEndSink>(on_end));
    serd_reader_read_file(reader, reinterpret_cast<const uint8_t *>(path.c_str()));
    serd_reader_free(reader);
}

void Dataset::add_ttl_file(const std::string &path) {
    // TODO: this is provisional until a native parser is implemented
    import_rdf_file(path, this);
}

}  // namespace rdf4cpp::rdf