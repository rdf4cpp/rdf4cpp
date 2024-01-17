#ifndef RDF4CPP_SERIALIZATION_HPP
#define RDF4CPP_SERIALIZATION_HPP

#include <rdf4cpp/rdf/Literal.hpp>
#include <rdf4cpp/rdf/Quad.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>
#include <rdf4cpp/rdf/writer/SerializationState.hpp>

namespace rdf4cpp::rdf::writer {
constexpr bool format_has_graph(OutputFormat f) {
    return f == OutputFormat::NQuads || f == OutputFormat::TriG;
}
constexpr bool format_has_prefix(OutputFormat f) {
    return f == OutputFormat::Turtle || f == OutputFormat::TriG;
}

template<OutputFormat F>
bool serialize_state(void *const buffer, Cursor &cursor, FlushFunc const flush, SerializationState *state) {
    if constexpr (format_has_prefix(F)) {
        if (state == nullptr)
            return false;
        for (const auto &p : state->prefixes) {
            if (!write_str("@prefix ", buffer, &cursor, flush))
                return false;
            if (!write_str(p.shorthand, buffer, &cursor, flush))
                return false;
            if (!write_str(": <", buffer, &cursor, flush))
                return false;
            if (!write_str(p.prefix, buffer, &cursor, flush))
                return false;
            if (!write_str("> .\n", buffer, &cursor, flush))
                return false;
        }
    }
    return true;
}
template<OutputFormat F>
bool flush_state(void *const buffer, Cursor &cursor, FlushFunc const flush, SerializationState *state) {
    if constexpr (format_has_prefix(F)) {
        if (state == nullptr)
            return false;
        if (!state->active_predicate.null() || !state->active_subject.null())
            if (!write_str(" .\n", buffer, &cursor, flush))
                return false;
        if (!state->active_graph.null())
            if (!write_str("}\n", buffer, &cursor, flush))
                return false;
        state->active_predicate = Node::make_null();
        state->active_subject = Node::make_null();
        state->active_graph = Node::make_null();
    }
    return true;
}

template<OutputFormat F>
bool serialize(const Quad &s, void *const buffer, Cursor &cursor, FlushFunc const flush, SerializationState *state) {
    if constexpr (format_has_prefix(F)) {
        if (state == nullptr)
            return false;
        if constexpr (format_has_graph(F)) {
            if (s.graph() != state->active_graph) {
                if (!flush_state<F>(buffer, cursor, flush, state))
                    return false;
                if (!s.graph().null()) {
                    if (!s.graph().serialize(buffer, &cursor, flush, state)) {
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
                if (!s.object().serialize(buffer, &cursor, flush, state))
                    return false;
                return true;
            }
            if (!write_str(" ;\n", buffer, &cursor, flush))
                return false;
            if (!s.predicate().serialize(buffer, &cursor, flush, state))
                return false;
            state->active_predicate = s.predicate();
            if (!write_str(" ", buffer, &cursor, flush))
                return false;
            if (!s.object().serialize(buffer, &cursor, flush, state))
                return false;
            return true;
        }
        if (!state->active_subject.null()) {
            if (!write_str(" .\n", buffer, &cursor, flush))
                return false;
        }
        if (!s.subject().serialize(buffer, &cursor, flush, state))
            return false;
        state->active_subject = s.subject();
        if (!write_str(" ", buffer, &cursor, flush))
            return false;
        if (!s.predicate().serialize(buffer, &cursor, flush, state))
            return false;
        state->active_predicate = s.predicate();
        if (!write_str(" ", buffer, &cursor, flush))
            return false;
        if (!s.object().serialize(buffer, &cursor, flush, state))
            return false;
        return true;
    } else {
        if (!s.subject().serialize(buffer, &cursor, flush))
            return false;
        if (!write_str(" ", buffer, &cursor, flush))
            return false;
        if (!s.predicate().serialize(buffer, &cursor, flush))
            return false;
        if (!write_str(" ", buffer, &cursor, flush))
            return false;
        if (!s.object().serialize(buffer, &cursor, flush))
            return false;
        if constexpr (format_has_graph(F)) {
            if (!s.graph().null()) {
                if (!write_str(" ", buffer, &cursor, flush))
                    return false;
                if (!s.graph().serialize(buffer, &cursor, flush)) {
                    return false;
                }
            }
        }
        if (!write_str(" .\n", buffer, &cursor, flush))
            return false;
    }
    return true;
}

inline bool serialize_iri_prefixed(std::string_view iri, void *const buffer, Cursor &cursor, FlushFunc const flush, const SerializationState *state) {
    if (state != nullptr) {
        for (const auto &p : state->prefixes) {
            if (iri.starts_with(p.prefix)) {
                if (!write_str(p.shorthand, buffer, &cursor, flush))
                    return false;
                if (!write_str(":", buffer, &cursor, flush))
                    return false;
                if (!write_str(iri.substr(p.prefix.size()), buffer, &cursor, flush))
                    return false;
                return true;
            }
        }
    }
    if (!write_str("<", buffer, &cursor, flush))
        return false;
    if (!write_str(iri, buffer, &cursor, flush))
        return false;
    if (!write_str(">", buffer, &cursor, flush))
        return false;
    return true;
}

}  // namespace rdf4cpp::rdf::writer
#endif  //RDF4CPP_SERIALIZATION_HPP
