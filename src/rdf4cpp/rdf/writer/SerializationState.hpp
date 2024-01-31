#ifndef RDF4CPP_SERIALIZATIONSTATE_HPP
#define RDF4CPP_SERIALIZATIONSTATE_HPP

#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/writer/BufWriter.hpp>
#include <rdf4cpp/rdf/Node.hpp>
#include <rdf4cpp/rdf/Quad.hpp>
#include <string>
#include <vector>

namespace rdf4cpp::rdf::writer {
enum class OutputFormat {
    NTriples,
    Turtle,
    NQuads,
    TriG
};
template<OutputFormat F>
concept format_has_graph = (F == OutputFormat::NQuads || F == OutputFormat::TriG);

template<OutputFormat F>
concept format_has_prefix = (F == OutputFormat::Turtle || F == OutputFormat::TriG);

struct SerializationState {
    Node active_graph;
    Node active_subject;
    Node active_predicate;

    static bool begin(void *buffer, Cursor &cursor, FlushFunc flush);
    bool flush(void *buffer, Cursor &cursor, FlushFunc flush);
};

struct TypeIRIPrefix {
    std::string_view prefix;
    std::string_view shorthand;
};
static constexpr std::array iri_prefixes = {
        TypeIRIPrefix{"http://www.w3.org/2001/XMLSchema#", "xsd"},
        TypeIRIPrefix{"http://www.w3.org/1999/02/22-rdf-syntax-ns#", "rdf"},
};
}  // namespace rdf4cpp::rdf::writer

#endif  //RDF4CPP_SERIALIZATIONSTATE_HPP
