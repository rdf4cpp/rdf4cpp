#ifndef RDF4CPP_NQUADSWRITER_HPP
#define RDF4CPP_NQUADSWRITER_HPP


#include <rdf4cpp/rdf/Dataset.hpp>
#include <rdf4cpp/rdf/writer/NNodeWriter.hpp>

#include <ostream>
#include <string>

namespace rdf4cpp::rdf::writer {

class NQuadsWriter {
    Dataset dataset_;

public:
    explicit NQuadsWriter(Dataset dataset);

    operator std::string() const;

    friend std::ostream &operator<<(std::ostream &os, const NQuadsWriter &writer);
    ;
};
}  // namespace rdf4cpp::rdf::writer
#endif  //RDF4CPP_NQUADSWRITER_HPP
