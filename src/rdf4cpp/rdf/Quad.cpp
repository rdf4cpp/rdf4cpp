#include "Quad.hpp"
#include <rdf4cpp/rdf/writer/SerializationState.hpp>
#include <rdf4cpp/rdf/writer/OutputFormat.hpp>
#include <rdf4cpp/rdf/writer/TryWrite.hpp>

namespace rdf4cpp::rdf {
Quad::Quad(Node subject, Node predicate, Node object) : QuadPattern(IRI::default_graph(), subject, predicate, object) {}
Quad::Quad(Node graph, Node subject, Node predicate, Node object) : QuadPattern(graph, subject, predicate, object) {}

bool Quad::valid() const noexcept {
    return (graph().is_iri() || (graph().is_blank_node() && !graph().null()))
            && ((subject().is_iri() || subject().is_blank_node()) && !subject().null())
            && (predicate().is_iri() && !predicate().null())
            && ((object().is_iri() || object().is_literal() || object().is_blank_node()) && !object().null());
}

std::optional<Quad> Quad::create_validated(Node graph, Node subject, Node predicate, Node object) noexcept {
    Quad quad{graph, subject, predicate, object};
    if (quad.valid()) {
        return quad;
    } else {
        return std::nullopt;
    }
}

std::optional<Quad> Quad::create_validated(Node subject, Node predicate, Node object) noexcept {
    Quad quad{subject, predicate, object};
    if (quad.valid()) {
        return quad;
    } else {
        return std::nullopt;
    }
}

Quad Quad::to_node_storage(storage::DynNodeStorage node_storage) const noexcept {
    Quad qu;
    auto it = qu.begin();
    for (auto const item : *this) {
        *(it++) = item.to_node_storage(node_storage);
    }
    return qu;
}

template<writer::OutputFormat F>
static bool write_node(Node const node, writer::BufWriterParts const writer) noexcept {
    if constexpr (writer::format_has_prefix<F>) {
        return node.serialize_short_form(writer);
    } else {
        return node.serialize(writer);
    }
}

template<writer::OutputFormat F>
static bool write_pred(Node const pred, writer::BufWriterParts const writer) {
    assert(pred.is_iri());

    if constexpr (writer::format_has_prefix<F>) {
        static constexpr storage::identifier::LiteralType rdf_type = datatypes::registry::reserved_datatype_ids[datatypes::registry::rdf_type];

        if (iri_node_id_to_literal_type(pred.backend_handle().node_id()) == rdf_type) {
            return writer::write_str("a", writer);
        }
    }

    return write_node<F>(pred, writer);
}

#define RDF4CPP_DETAIL_TRY_WRITE_NODE(pred) \
    if (!write_node<F>((pred), writer)) {   \
        return false;                       \
    }

#define RDF4CPP_DETAIL_TRY_WRITE_PRED(pred) \
    if (!write_pred<F>((pred), writer)) {   \
        return false;                       \
    }

template<writer::OutputFormat F>
static bool serialize(Quad const &s, writer::BufWriterParts const writer, writer::SerializationState *const state) noexcept {
    if constexpr (writer::format_has_prefix<F>) {
        if constexpr (writer::format_has_graph<F>) {
            if (s.graph() != state->active_graph) {
                if (!state->flush(writer)) {
                    return false;
                }

                if (!s.graph().null()) {
                    RDF4CPP_DETAIL_TRY_WRITE_NODE(s.graph());
                    RDF4CPP_DETAIL_TRY_WRITE_STR(" {\n");

                    state->active_graph = s.graph();
                }
            }
        }

        if (!state->active_subject.null() && state->active_subject == s.subject()) {
            if (!state->active_predicate.null() && state->active_predicate == s.predicate()) {
                RDF4CPP_DETAIL_TRY_WRITE_STR(" ,\n");
                RDF4CPP_DETAIL_TRY_WRITE_NODE(s.object());
                return true;
            }

            RDF4CPP_DETAIL_TRY_WRITE_STR(" ;\n");
            RDF4CPP_DETAIL_TRY_WRITE_PRED(s.predicate());

            state->active_predicate = s.predicate();
            RDF4CPP_DETAIL_TRY_WRITE_STR(" ");
            RDF4CPP_DETAIL_TRY_WRITE_NODE(s.object());
            return true;
        }

        if (!state->active_subject.null()) {
            RDF4CPP_DETAIL_TRY_WRITE_STR(" .\n");
        }

        state->active_subject = s.subject();
        state->active_predicate = s.predicate();
    }

    RDF4CPP_DETAIL_TRY_WRITE_NODE(s.subject());
    RDF4CPP_DETAIL_TRY_WRITE_STR(" ");
    RDF4CPP_DETAIL_TRY_WRITE_PRED(s.predicate());
    RDF4CPP_DETAIL_TRY_WRITE_STR(" ");
    RDF4CPP_DETAIL_TRY_WRITE_NODE(s.object());

    if constexpr (!writer::format_has_prefix<F>) {
        if constexpr (writer::format_has_graph<F>) {
            if (!s.graph().null()) {
                RDF4CPP_DETAIL_TRY_WRITE_STR(" ");
                RDF4CPP_DETAIL_TRY_WRITE_NODE(s.graph());
            }
        }

        RDF4CPP_DETAIL_TRY_WRITE_STR(" .\n");
    }

    return true;
}

#undef RDF4CPP_DETAIL_TRY_WRITE_NODE
#undef RDF4CPP_DETAIL_TRY_WRITE_PRED

bool Quad::serialize_ntriples(writer::BufWriterParts const writer) const noexcept {
    return serialize<writer::OutputFormat::NTriples>(*this, writer, nullptr);
}

bool Quad::serialize_nquads(writer::BufWriterParts const writer) const noexcept {
    return serialize<writer::OutputFormat::NQuads>(*this, writer, nullptr);
}

bool Quad::serialize_turtle(writer::SerializationState &state, writer::BufWriterParts const writer) const noexcept {
    return serialize<writer::OutputFormat::Turtle>(*this, writer, &state);
}

bool Quad::serialize_trig(writer::SerializationState &state, writer::BufWriterParts const writer) const noexcept {
    return serialize<writer::OutputFormat::TriG>(*this, writer, &state);
}

}  // namespace rdf4cpp::rdf