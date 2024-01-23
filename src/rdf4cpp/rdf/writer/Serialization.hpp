#ifndef RDF4CPP_SERIALIZATION_HPP
#define RDF4CPP_SERIALIZATION_HPP

#include <functional>

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

struct TypeIRIPrefix {
    std::string_view prefix;
    std::string_view shorthand;
};
static constexpr std::array iri_prefixes = {
        TypeIRIPrefix{"http://www.w3.org/2001/XMLSchema#", "xsd"},
        TypeIRIPrefix{"http://www.w3.org/1999/02/22-rdf-syntax-ns#", "rdf"},
};

template<OutputFormat F>
bool write_prefix(void *const buffer, Cursor &cursor, FlushFunc const flush) {
    if constexpr (format_has_prefix(F)) {
        for (const auto &p : iri_prefixes) {
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
    auto seri = &Node::serialize_short_form;
    if constexpr (!format_has_prefix(F))
        seri = &Node::serialize;
    if constexpr (format_has_prefix(F)) {
        if (state == nullptr)
            return false;
        if constexpr (format_has_graph(F)) {
            if (s.graph() != state->active_graph) {
                if (!flush_state<F>(buffer, cursor, flush, state))
                    return false;
                if (!s.graph().null()) {
                    if (!std::invoke(seri, s.graph(), buffer, &cursor, flush)) {
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
                if (!std::invoke(seri, s.object(), buffer, &cursor, flush))
                    return false;
                return true;
            }
            if (!write_str(" ;\n", buffer, &cursor, flush))
                return false;
            if (!std::invoke(seri, s.predicate(), buffer, &cursor, flush))
                return false;
            state->active_predicate = s.predicate();
            if (!write_str(" ", buffer, &cursor, flush))
                return false;
            if (!std::invoke(seri, s.object(), buffer, &cursor, flush))
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
    if (!std::invoke(seri, s.subject(), buffer, &cursor, flush))
        return false;
    if (!write_str(" ", buffer, &cursor, flush))
        return false;
    if (!std::invoke(seri, s.predicate(), buffer, &cursor, flush))
        return false;
    if (!write_str(" ", buffer, &cursor, flush))
        return false;
    if (!std::invoke(seri, s.object(), buffer, &cursor, flush))
        return false;
    if constexpr (format_has_graph(F) && !format_has_prefix(F)) {
        if (!s.graph().null()) {
            if (!write_str(" ", buffer, &cursor, flush))
                return false;
            if (!std::invoke(seri, s.graph(), buffer, &cursor, flush)) {
                return false;
            }
        }
    }
    if constexpr (!format_has_prefix(F))
        if (!write_str(" .\n", buffer, &cursor, flush))
            return false;
    return true;
}

static consteval size_t type_iri_get_size() {
    uint8_t m = 0;
    for (const auto &[name, id] : datatypes::registry::reserved_datatype_ids) {
        if (id.to_underlying() > m)
            m = id.to_underlying();
    }
    return m;
}
static consteval auto type_iri_build_buffer() {
    std::array<TypeIRIPrefix, type_iri_get_size()+1> buffer{};
    for (const auto &[name, id] : datatypes::registry::reserved_datatype_ids) {
        size_t index = id.to_underlying();
        for (const auto& c : iri_prefixes) {
            if (name.starts_with(c.prefix)) {
                buffer[index] = c;
                break;
            }
        }
    }
    return buffer;
}
static constexpr auto type_iri_buffer = type_iri_build_buffer();
inline bool write_type_iri(datatypes::registry::LiteralType t, void *const buffer, writer::Cursor *cursor, writer::FlushFunc const flush, bool short_form) {
    if (short_form) {
        auto& p = type_iri_buffer.at(t.to_underlying());
        if (!p.prefix.empty()) { // TODO precompute full prefixed IRI?
            if (!write_str(p.shorthand, buffer, cursor, flush))
                return false;
            if (!write_str(":", buffer, cursor, flush))
                return false;
            const auto& iri = datatypes::registry::DatatypeRegistry::get_entry(t)->datatype_iri;
            if (!write_str(iri.substr(p.prefix.size()), buffer, cursor, flush))
                return false;
            return true;
        }
    }
    if (!write_str("<", buffer, cursor, flush))
        return false;
    if (!write_str(datatypes::registry::DatatypeRegistry::get_entry(t)->datatype_iri, buffer, cursor, flush))
        return false;
    if (!write_str(">", buffer, cursor, flush))
        return false;
    return true;
}

}  // namespace rdf4cpp::rdf::writer
#endif  //RDF4CPP_SERIALIZATION_HPP
