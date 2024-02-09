#include "SerializationState.hpp"

namespace rdf4cpp::rdf::writer {
bool SerializationState::begin(void *buffer, Cursor *cursor, FlushFunc flush) {
    for (const auto& p : iri_prefixes) {
        if (!write_str("@prefix ", buffer, cursor, flush))
            return false;
        if (!write_str(p.shorthand, buffer, cursor, flush))
            return false;
        if (!write_str(": <", buffer, cursor, flush))
            return false;
        if (!write_str(p.prefix, buffer, cursor, flush))
            return false;
        if (!write_str("> .\n", buffer, cursor, flush))
            return false;
    }
    return true;
}
bool SerializationState::flush(void *buffer, Cursor *cursor, FlushFunc flush) {
    if (!active_predicate.null() || !active_subject.null())
        if (!write_str(" .\n", buffer, cursor, flush))
            return false;
    if (!active_graph.null())
        if (!write_str("}\n", buffer, cursor, flush))
            return false;
    active_predicate = Node::make_null();
    active_subject = Node::make_null();
    active_graph = Node::make_null();
    return true;
}
}