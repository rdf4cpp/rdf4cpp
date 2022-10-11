#include <rdf4cpp/rdf/parser/private/IStreamQuadIteratorSerdImpl.hpp>

#include <string_view>

namespace rdf4cpp::rdf::parser {

namespace util {

/**
 * Adaptor function so that serd can read from std::istreams.
 * Matches the interface of SerdSource
 */
static size_t istream_read(void *buf, [[maybe_unused]] size_t elem_size, size_t count, void *voided_self) noexcept {
    assert(elem_size == 1);

    auto *self = reinterpret_cast<std::istream *>(voided_self);
    self->read(static_cast<char *>(buf), static_cast<std::streamsize>(count));
    return self->gcount();
}

/**
 * Adaptor function for serd to check if an std::istream is ok
 * Matches the interface of SerdStreamErrorFunc
 */
static int istream_is_ok(void *voided_self) noexcept {
    auto *self = reinterpret_cast<std::istream *>(voided_self);
    return *self ? 0 : 1;
}

} // namespace util

std::string_view IStreamQuadIterator::Impl::node_into_string_view(SerdNode const *node) noexcept {
    return std::string_view{reinterpret_cast<char const *>(node->buf), static_cast<size_t>(node->n_bytes)};
}

ParsingError::Type IStreamQuadIterator::Impl::parsing_error_type_from_serd(SerdStatus const st) noexcept {
    switch (st) {
        case SerdStatus::SERD_ERR_BAD_SYNTAX:
            return ParsingError::Type::BadSyntax;
        case SerdStatus::SERD_ERR_BAD_CURIE:
            return ParsingError::Type::BadCurie;
        case SerdStatus::SERD_FAILURE:
            return ParsingError::Type::EofReached;
        case SerdStatus::SERD_ERR_ID_CLASH:
            return ParsingError::Type::BlankNodeIdClash;
        default:
            return ParsingError::Type::Internal;
    }
}

BlankNode IStreamQuadIterator::Impl::get_bnode(SerdNode const *node) const {
    return BlankNode{node_into_string_view(node), this->node_storage};
}

IRI IStreamQuadIterator::Impl::get_uri(SerdNode const *node) const {
    return IRI{node_into_string_view(node), this->node_storage};
}

nonstd::expected<IRI, SerdStatus> IStreamQuadIterator::Impl::get_prefixed_uri(SerdNode const *node) const {
    auto const uri_node_view = node_into_string_view(node);

    auto const sep_pos = uri_node_view.find(':');
    if (sep_pos == std::string_view::npos) {
        return nonstd::make_unexpected(SerdStatus::SERD_ERR_BAD_CURIE);
    }

    auto const prefix = uri_node_view.substr(0, sep_pos);
    auto const suffix = uri_node_view.substr(sep_pos + 1);

    if (auto const prefix_it = this->prefixes.find(prefix); prefix_it != this->prefixes.end()) {
        std::ostringstream oss;
        oss << prefix_it->second << suffix;
        return IRI{oss.view(), this->node_storage};
    } else {
        return nonstd::make_unexpected(SerdStatus::SERD_ERR_BAD_CURIE);
    }
}

Literal IStreamQuadIterator::Impl::get_literal(SerdNode const *literal, SerdNode const *datatype, SerdNode const *lang) const {
    auto const literal_value = node_into_string_view(literal);

    if (datatype != nullptr) {
        return Literal{literal_value, IRI{node_into_string_view(datatype), this->node_storage}, this->node_storage};
    } else if (lang != nullptr) {
        return Literal{literal_value, node_into_string_view(lang), this->node_storage};
    } else {
        return Literal{literal_value, this->node_storage};
    }
}

SerdStatus IStreamQuadIterator::Impl::on_error(void *voided_self, SerdError const *error) noexcept {
    auto *self = reinterpret_cast<Impl *>(voided_self);

    auto const buf_sz = vsnprintf(nullptr, 0, error->fmt, *error->args);
    std::string message;

    message.resize(buf_sz + 1); // +1 for null-terminator
    vsnprintf(message.data(), message.size(), error->fmt, *error->args);
    message.resize(buf_sz - 1); // drop null-terminator from vsnprintf and newline from serd

    self->last_error = ParsingError{
            .error_type = parsing_error_type_from_serd(error->status),
            .line = error->line,
            .col = error->col,
            .message = message};

    return SerdStatus::SERD_SUCCESS;
}

SerdStatus IStreamQuadIterator::Impl::on_base(void *voided_self, const SerdNode *uri) noexcept {
    auto *self = reinterpret_cast<Impl *>(voided_self);
    self->prefixes.emplace("", node_into_string_view(uri));
    self->last_read_success = true;
    return SERD_SUCCESS;
}

SerdStatus IStreamQuadIterator::Impl::on_prefix(void *voided_self, SerdNode const *name, SerdNode const *uri) noexcept {
    auto *self = reinterpret_cast<Impl *>(voided_self);
    self->prefixes.emplace(node_into_string_view(name), node_into_string_view(uri));
    self->last_read_success = true;
    return SERD_SUCCESS;
}

SerdStatus IStreamQuadIterator::Impl::on_stmt(void *voided_self,
                                              SerdStatementFlags,
                                              SerdNode const *graph,
                                              SerdNode const *subj,
                                              SerdNode const *pred,
                                              SerdNode const *obj,
                                              SerdNode const *obj_datatype,
                                              SerdNode const *obj_lang) noexcept {

    auto *self = reinterpret_cast<Impl *>(voided_self);

    auto const graph_node = [&]() -> nonstd::expected<Node, SerdStatus> {
        if (graph != nullptr) {
            switch (graph->type) {
                case SERD_CURIE:
                    return self->get_prefixed_uri(pred);
                case SERD_URI:
                    return self->get_uri(subj);
                case SERD_BLANK:
                    return self->get_bnode(subj);
                default:
                    return nonstd::make_unexpected(SERD_ERR_BAD_SYNTAX);
            }
        } else {
            return IRI::default_graph();
        }
    }();

    if (!graph_node.has_value()) {
        return graph_node.error();
    }

    auto const subj_node = [&]() -> nonstd::expected<Node, SerdStatus> {
        switch (subj->type) {
            case SERD_CURIE:
                return self->get_prefixed_uri(subj);
            case SERD_URI:
                return self->get_uri(subj);
            case SERD_BLANK:
                return self->get_bnode(subj);
            default:
                return nonstd::make_unexpected(SERD_ERR_BAD_SYNTAX);
        }
    }();

    if (!subj_node.has_value()) {
        return subj_node.error();
    }

    auto const pred_node = [&]() -> nonstd::expected<Node, SerdStatus> {
        switch (pred->type) {
            case SERD_CURIE:
                return self->get_prefixed_uri(pred);
            case SERD_URI:
                return self->get_uri(pred);
            default:
                return nonstd::make_unexpected(SERD_ERR_BAD_SYNTAX);
        }
    }();

    if (!pred_node.has_value()) {
        return pred_node.error();
    }

    auto const obj_node = [&]() -> nonstd::expected<Node, SerdStatus> {
        switch (obj->type) {
            case SERD_CURIE:
                return self->get_prefixed_uri(pred);
            case SERD_LITERAL:
                return self->get_literal(obj, obj_datatype, obj_lang);
            case SERD_BLANK:
                return self->get_bnode(obj);
            case SERD_URI:
                return self->get_uri(obj);
            default:
                return nonstd::make_unexpected(SERD_ERR_BAD_SYNTAX);
        }
    }();

    if (!obj_node.has_value()) {
        return obj_node.error();
    }

    self->quad_buffer.emplace_back(*graph_node, *subj_node, *pred_node, *obj_node);
    self->last_read_success = true;
    return SERD_SUCCESS;
}

IStreamQuadIterator::Impl::Impl(std::istream &istream, bool strict, storage::node::NodeStorage node_storage) noexcept
    : istream{std::ref(istream)},
      node_storage{std::move(node_storage)},
      reader{serd_reader_new(SerdSyntax::SERD_TURTLE, this, nullptr, &Impl::on_base, &Impl::on_prefix, &Impl::on_stmt, nullptr)} {

    serd_reader_set_strict(this->reader.get(), strict);
    serd_reader_set_error_sink(this->reader.get(), &Impl::on_error, this);
    serd_reader_start_source_stream(this->reader.get(), &util::istream_read, &util::istream_is_ok, &this->istream.get(), nullptr, 4096);
}

std::optional<nonstd::expected<Quad, ParsingError>> IStreamQuadIterator::Impl::next() noexcept {
    if (this->is_at_end()) [[unlikely]] {
        return std::nullopt;
    }

    while (this->quad_buffer.empty()) {
        this->last_error = std::nullopt;
        this->last_read_success = false; // will be set by the callbacks
        serd_reader_read_chunk(this->reader.get());

        if (!this->last_read_success) {
            // was not able to read stmt, prefix or base

            if (!this->last_error.has_value()) {
                // did not receive error either => must be eof
                this->end_flag = true;
                return std::nullopt; // eof reached
            }

            serd_reader_skip_error(this->reader.get());
            return nonstd::make_unexpected(*this->last_error);
        }
    }

    auto ret = this->quad_buffer.front();
    this->quad_buffer.pop_front();
    return ret;
}

}  // namespace rdf4cpp::rdf::parser
