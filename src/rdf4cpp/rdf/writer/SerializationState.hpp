#ifndef RDF4CPP_SERIALIZATIONSTATE_HPP
#define RDF4CPP_SERIALIZATIONSTATE_HPP

#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf::writer {
enum struct OutputFormat {
    NTriples,
    Turtle,
    NQuads,
    TriG
};

struct SerializationState {
    Node active_graph;
    Node active_subject;
    Node active_predicate;
};
}  // namespace rdf4cpp::rdf::writer

#endif  //RDF4CPP_SERIALIZATIONSTATE_HPP
