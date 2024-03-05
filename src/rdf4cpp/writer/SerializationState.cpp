#include "SerializationState.hpp"
#include <rdf4cpp/writer/Prefixes.hpp>
#include <rdf4cpp/writer/TryWrite.hpp>

namespace rdf4cpp::rdf::writer {

bool SerializationState::begin(BufWriterParts const writer) noexcept {
    for (auto const &p : iri_prefixes) {
        RDF4CPP_DETAIL_TRY_WRITE_STR("@prefix ");
        RDF4CPP_DETAIL_TRY_WRITE_STR(p.shorthand);
        RDF4CPP_DETAIL_TRY_WRITE_STR(": <");
        RDF4CPP_DETAIL_TRY_WRITE_STR(p.prefix);
        RDF4CPP_DETAIL_TRY_WRITE_STR("> .\n");
    }

    return true;
}

bool SerializationState::flush(BufWriterParts const writer) noexcept {
    if (!active_predicate.null() || !active_subject.null()) {
        RDF4CPP_DETAIL_TRY_WRITE_STR(" .\n");
    }

    if (!active_graph.null()) {
        RDF4CPP_DETAIL_TRY_WRITE_STR("}\n");
    }

    active_predicate = Node::make_null();
    active_subject = Node::make_null();
    active_graph = Node::make_null();
    return true;
}

} // namespace rdf4cpp::rdf::writer
