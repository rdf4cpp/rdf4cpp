#include "NNodeWriter.hpp"
namespace rdf4cpp::rdf::writer {

NNodeWriter::NNodeWriter(Node node) : node_(node) {}
NNodeWriter::operator std::string() const {
    return (std::string) node_;
}
std::ostream &operator<<(std::ostream &os, const NNodeWriter &writer) {
    os << writer.node_;
    return os;
}
}  // namespace rdf4cpp::rdf::writer