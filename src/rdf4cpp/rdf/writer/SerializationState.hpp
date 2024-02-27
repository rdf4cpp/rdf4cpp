#ifndef RDF4CPP_SERIALIZATIONSTATE_HPP
#define RDF4CPP_SERIALIZATIONSTATE_HPP

#include <rdf4cpp/rdf/Node.hpp>

namespace rdf4cpp::rdf::writer {

struct SerializationState {
    Node active_graph;
    Node active_subject;
    Node active_predicate;

    static bool begin(void *buffer, Cursor *cursor, FlushFunc flush) noexcept;
    bool flush(void *buffer, Cursor *cursor, FlushFunc flush) noexcept;

    template<writer::BufWriter W>
    static bool begin(W &w) noexcept {
        return begin(&w.buffer(), &w.cursor(), &W::flush);
    }

    template<writer::BufWriter W>
    bool flush(W &w) noexcept {
        return flush(&w.buffer(), &w.cursor(), &W::flush);
    }
};

}  // namespace rdf4cpp::rdf::writer

#endif  //RDF4CPP_SERIALIZATIONSTATE_HPP
