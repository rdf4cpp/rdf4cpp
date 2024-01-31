#include "Quad.hpp"
#include <rdf4cpp/rdf/writer/SerializationState.hpp>

namespace rdf4cpp::rdf {
Quad::Quad(Node subject, Node predicate, Node object) : QuadPattern(IRI::default_graph(), subject, predicate, object) {}
Quad::Quad(Node graph, Node subject, Node predicate, Node object) : QuadPattern(graph, subject, predicate, object) {}
bool Quad::valid() const {
    return ((graph().is_iri() or (graph().is_blank_node() and not graph().null())) and
            (subject().is_iri() or subject().is_blank_node()) and not subject().null() and
            (predicate().is_iri()) and not predicate().null() and
            (object().is_iri() or object().is_literal() or object().is_blank_node()) and not object().null());
}
std::optional<Quad> Quad::create_validated(Node graph, Node subject, Node predicate, Node object) {
    Quad quad{graph, subject, predicate, object};
    if (quad.valid())
        return quad;
    else
        return std::nullopt;
}
std::optional<Quad> Quad::create_validated(Node subject, Node predicate, Node object) {
    Quad quad{subject, predicate, object};
    if (quad.valid())
        return quad;
    else
        return std::nullopt;
}

template<writer::OutputFormat F>
bool write_node(Node n, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) {
    if constexpr (writer::format_has_prefix<F>)
        return n.serialize_short_form(buffer, cursor, flush);
    else
        return n.serialize(buffer, cursor, flush);
}
template<writer::OutputFormat F>
bool serialize(const Quad &s, void *const buffer, writer::Cursor &cursor, writer::FlushFunc const flush, writer::SerializationState *state) {
    auto write_predicate = [&](const Node &p) {
        if constexpr (writer::format_has_prefix<F>) {
            static constexpr auto rdf_type = datatypes::registry::reserved_datatype_ids[datatypes::registry::rdf_type];
            if (p.is_iri() && iri_node_id_to_literal_type(p.backend_handle().node_id()) == rdf_type)
                return write_str("a", buffer, &cursor, flush);
        }
        return write_node<F>(p, buffer, &cursor, flush);
    };
    if constexpr (writer::format_has_prefix<F>) {
        if constexpr (writer::format_has_graph<F>) {
            if (s.graph() != state->active_graph) {
                if (!state->flush(buffer, cursor, flush))
                    return false;
                if (!s.graph().null()) {
                    if (!write_node<F>(s.graph(), buffer, &cursor, flush)) {
                        return false;
                    }
                    if (!write_str(" {\n", buffer, &cursor, flush))
                        return false;
                    state->active_graph = s.graph();
                }
            }
        }
        if (!state->active_subject.null() && state->active_subject == s.subject()) {
            if (!state->active_predicate.null() && state->active_predicate == s.predicate()) {
                if (!write_str(" ,\n", buffer, &cursor, flush))
                    return false;
                if (!write_node<F>(s.object(), buffer, &cursor, flush))
                    return false;
                return true;
            }
            if (!write_str(" ;\n", buffer, &cursor, flush))
                return false;
            if (!write_predicate(s.predicate()))
                return false;
            state->active_predicate = s.predicate();
            if (!write_str(" ", buffer, &cursor, flush))
                return false;
            if (!write_node<F>(s.object(), buffer, &cursor, flush))
                return false;
            return true;
        }
        if (!state->active_subject.null()) {
            if (!write_str(" .\n", buffer, &cursor, flush))
                return false;
        }
        state->active_subject = s.subject();
        state->active_predicate = s.predicate();
    }
    else {
        state = nullptr;
    }
    if (!write_node<F>(s.subject(), buffer, &cursor, flush))
        return false;
    if (!write_str(" ", buffer, &cursor, flush))
        return false;
    if (!write_predicate(s.predicate()))
        return false;
    if (!write_str(" ", buffer, &cursor, flush))
        return false;
    if (!write_node<F>(s.object(), buffer, &cursor, flush))
        return false;
    if constexpr (writer::format_has_graph<F> && !writer::format_has_prefix<F>) {
        if (!s.graph().null()) {
            if (!write_str(" ", buffer, &cursor, flush))
                return false;
            if (!write_node<F>(s.graph(), buffer, &cursor, flush)) {
                return false;
            }
        }
    }
    if constexpr (!writer::format_has_prefix<F>)
        if (!write_str(" .\n", buffer, &cursor, flush))
            return false;
    return true;
}

bool Quad::serialize_ntriples(void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const {
    return serialize<writer::OutputFormat::NTriples>(*this, buffer, *cursor, flush, nullptr);
}
bool Quad::serialize_nquad(void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const {
    return serialize<writer::OutputFormat::NQuads>(*this, buffer, *cursor, flush, nullptr);
}
bool Quad::serialize_turtle(writer::SerializationState &state, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const {
    return serialize<writer::OutputFormat::Turtle>(*this, buffer, *cursor, flush, &state);
}
bool Quad::serialize_trig(writer::SerializationState &state, void *buffer, writer::Cursor *cursor, writer::FlushFunc flush) const {
    return serialize<writer::OutputFormat::TriG>(*this, buffer, *cursor, flush, &state);
}
}  // namespace rdf4cpp::rdf