#include "NQuadsWriter.hpp"

#include <sstream>


namespace rdf4cpp::rdf::writer {


NQuadsWriter::NQuadsWriter(Dataset dataset) : dataset_(std::move(dataset)) {}
NQuadsWriter::operator std::string() const {
    std::stringstream stream;
    stream << *this;
    return stream.str();
}
std::ostream &operator<<(std::ostream &os, const NQuadsWriter &writer) {
    IRI const default_graph = IRI::default_graph(writer.dataset_.backend().node_storage());

    for (const Quad &quad : writer.dataset_) {
        if (not(quad.graph().is_iri() and (static_cast<IRI>(quad.graph())) == default_graph))
            os << NNodeWriter(quad.graph()) << " ";
        os << NNodeWriter(quad.subject()) << " "
           << NNodeWriter(quad.predicate()) << " "
           << NNodeWriter(quad.object()) << " .\n";
    }
    return os;
}
}  // namespace rdf4cpp::rdf::writer