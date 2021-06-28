#ifndef RDF4CPP_NNODEWRITER_HPP
#define RDF4CPP_NNODEWRITER_HPP

#include <rdf4cpp/rdf/Node.hpp>

#include <ostream>
#include <string>

namespace rdf4cpp::rdf::writer {

class NNodeWriter {
    Node node_;

public:
    explicit NNodeWriter(Node node);

    operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const NNodeWriter &writer);
};
}  // namespace rdf4cpp::rdf::writer

#endif  //RDF4CPP_NNODEWRITER_HPP
