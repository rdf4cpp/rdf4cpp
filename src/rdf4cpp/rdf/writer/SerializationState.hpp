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

class SerializationState {
public:
    struct Prefix {
        std::string prefix, shorthand;
    };

    std::vector<Prefix> prefixes = {
            {"http://www.w3.org/2001/XMLSchema#", "xsd"},
    };

private:
    Node active_graph, active_subject, active_predicate;

    template<OutputFormat F>
    friend bool flush_state(void *const buffer, Cursor &cursor, FlushFunc const flush, SerializationState* state);
    template<OutputFormat F>
    friend bool serialize(const Quad &s, void *const buffer, Cursor &cursor, FlushFunc const flush, SerializationState* state);
};
}  // namespace rdf4cpp::rdf::writer

#endif  //RDF4CPP_SERIALIZATIONSTATE_HPP
