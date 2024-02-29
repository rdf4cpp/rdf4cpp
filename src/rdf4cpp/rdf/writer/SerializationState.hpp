#ifndef RDF4CPP_SERIALIZATIONSTATE_HPP
#define RDF4CPP_SERIALIZATIONSTATE_HPP

#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf::writer {

struct SerializationState {
    Node active_graph;
    Node active_subject;
    Node active_predicate;

    static bool begin(BufWriterParts parts) noexcept;
    bool flush(BufWriterParts parts) noexcept;
};

}  // namespace rdf4cpp::rdf::writer

#endif  //RDF4CPP_SERIALIZATIONSTATE_HPP
