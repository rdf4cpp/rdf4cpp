#include "NTriplesWriter.hpp"

#include <rdf4cpp/rdf/query/QuadPattern.hpp>

#include <sstream>

namespace rdf4cpp::rdf::writer {
NTriplesWriter::NTriplesWriter(Graph graph) : graph_(std::move(graph)) {}
NTriplesWriter::operator std::string() const {
    std::stringstream stream;
    stream << *this;
    return stream.str();
}
std::ostream &operator<<(std::ostream &os, const NTriplesWriter &writer) {
    using namespace query;
    auto solutions = writer.graph_.backend()->match(QuadPattern(writer.graph_.name(), Variable("s"), Variable("p"), Variable("o")));
    for (const auto &solution : solutions) {
        os << solution[0] << " "
           << solution[1] << " "
           << solution[2] << " .\n";
    }
    return os;
}
}  // namespace rdf4cpp::rdf::writer