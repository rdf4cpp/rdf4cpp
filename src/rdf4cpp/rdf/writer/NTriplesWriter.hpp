#ifndef RDF4CPP_NTRIPLESWRITER_HPP
#define RDF4CPP_NTRIPLESWRITER_HPP

#include <rdf4cpp/rdf/Graph.hpp>

#include <ostream>
#include <string>

namespace rdf4cpp::rdf::writer {
class NTriplesWriter {
    Graph graph_;

public:
    explicit NTriplesWriter(Graph graph);

    [[nodiscard]] explicit operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const NTriplesWriter &writer);
};
}  // namespace rdf4cpp::rdf::writer


#endif  //RDF4CPP_NTRIPLESWRITER_HPP
