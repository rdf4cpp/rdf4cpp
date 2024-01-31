#ifndef RDF4CPP_SERIALIZATION_HPP
#define RDF4CPP_SERIALIZATION_HPP

#include <functional>

#include <rdf4cpp/rdf/Literal.hpp>
#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>
#include <rdf4cpp/rdf/writer/SerializationState.hpp>

namespace rdf4cpp::rdf::writer {
template<OutputFormat F>
concept format_has_graph = (F == OutputFormat::NQuads || F == OutputFormat::TriG);

template<OutputFormat F>
concept format_has_prefix = (F == OutputFormat::Turtle || F == OutputFormat::TriG);


bool write_prefix_data(void *buffer, Cursor *cursor, FlushFunc flush);

template<OutputFormat F>
bool write_prefix(void *const buffer, Cursor *cursor, FlushFunc const flush) {
    if constexpr (format_has_prefix<F>) {
        return write_prefix_data(buffer, cursor, flush);
    }
    return true;
}

template<OutputFormat F>
bool flush_state(void *const buffer, Cursor *cursor, FlushFunc const flush, SerializationState *state) {
    if constexpr (format_has_prefix<F>) {
        if (state == nullptr)
            return false;
        if (!state->active_predicate.null() || !state->active_subject.null())
            if (!write_str(" .\n", buffer, cursor, flush))
                return false;
        if (!state->active_graph.null())
            if (!write_str("}\n", buffer, cursor, flush))
                return false;
        state->active_predicate = Node::make_null();
        state->active_subject = Node::make_null();
        state->active_graph = Node::make_null();
    }
    return true;
}

template<OutputFormat F>
bool serialize(const Quad &s, void *const buffer, Cursor *cursor, FlushFunc const flush, SerializationState *state) {
    auto write_node = &Node::serialize_short_form;
    if constexpr (!format_has_prefix<F>)
        write_node = &Node::serialize;
    auto write_predicate = [&](const Node &p) {
        if constexpr (format_has_prefix<F>) {
            static constexpr size_t rdf_type = datatypes::registry::reserved_datatype_ids[datatypes::registry::rdf_type].to_underlying();
            if (p.is_iri() && p.backend_handle().node_id().value() == rdf_type)
                return write_str("a", buffer, cursor, flush);
        }
        return std::invoke(write_node, p, buffer, cursor, flush);
    };
    if constexpr (format_has_prefix<F>) {
        if (state == nullptr)
            return false;
        if constexpr (format_has_graph<F>) {
            if (s.graph() != state->active_graph) {
                if (!flush_state<F>(buffer, cursor, flush, state))
                    return false;
                if (!s.graph().null()) {
                    if (!std::invoke(write_node, s.graph(), buffer, cursor, flush)) {
                        return false;
                    }
                    if (!write_str(" {\n", buffer, cursor, flush))
                        return false;
                    state->active_graph = s.graph();
                }
            }
        }
        if (!state->active_subject.null() && state->active_subject == s.subject()) {
            if (!state->active_predicate.null() && state->active_predicate == s.predicate()) {
                if (!write_str(" ,\n", buffer, cursor, flush))
                    return false;
                if (!std::invoke(write_node, s.object(), buffer, cursor, flush))
                    return false;
                return true;
            }
            if (!write_str(" ;\n", buffer, cursor, flush))
                return false;
            if (!write_predicate(s.predicate()))
                return false;
            state->active_predicate = s.predicate();
            if (!write_str(" ", buffer, cursor, flush))
                return false;
            if (!std::invoke(write_node, s.object(), buffer, cursor, flush))
                return false;
            return true;
        }
        if (!state->active_subject.null()) {
            if (!write_str(" .\n", buffer, cursor, flush))
                return false;
        }
        state->active_subject = s.subject();
        state->active_predicate = s.predicate();
    }
    else {
        state = nullptr;
    }
    if (!std::invoke(write_node, s.subject(), buffer, cursor, flush))
        return false;
    if (!write_str(" ", buffer, cursor, flush))
        return false;
    if (!write_predicate(s.predicate()))
        return false;
    if (!write_str(" ", buffer, cursor, flush))
        return false;
    if (!std::invoke(write_node, s.object(), buffer, cursor, flush))
        return false;
    if constexpr (format_has_graph<F> && !format_has_prefix<F>) {
        if (!s.graph().null()) {
            if (!write_str(" ", buffer, cursor, flush))
                return false;
            if (!std::invoke(write_node, s.graph(), buffer, cursor, flush)) {
                return false;
            }
        }
    }
    if constexpr (!format_has_prefix<F>)
        if (!write_str(" .\n", buffer, cursor, flush))
            return false;
    return true;
}

bool write_type_iri(datatypes::registry::LiteralType t, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush, bool short_form);

template<OutputFormat F>
class QuadSerializer {
    void *const buffer;
    Cursor *cursor;
    FlushFunc const flush_;
    SerializationState state = {};
    bool ok_;

public:
    QuadSerializer(void *b, Cursor *c, FlushFunc f) : buffer(b), cursor(c), flush_(f) {
        ok_ = write_prefix<F>(buffer, cursor, flush_);
    }
    template<BufWriter W>
    explicit QuadSerializer(W &w) : QuadSerializer(&w.buffer(), &w.cursor(), &W::flush) {
    }
    QuadSerializer(const QuadSerializer &) = delete;
    QuadSerializer(QuadSerializer &&) noexcept = default;
    QuadSerializer &operator=(const QuadSerializer &) = delete;
    QuadSerializer &operator=(QuadSerializer &&) noexcept = default;

    QuadSerializer &operator<<(const Quad &q) {
        ok_ = serialize<F>(q, buffer, cursor, flush_, &state);
        return *this;
    }

    [[nodiscard]] bool good() const noexcept {
        return ok_;
    }

    QuadSerializer &flush() {
        ok_ = flush_state<F>(buffer, cursor, flush_, &state);
        return *this;
    }

    ~QuadSerializer() {
        flush();
    }
};
}  // namespace rdf4cpp::rdf::writer
#endif  //RDF4CPP_SERIALIZATION_HPP
