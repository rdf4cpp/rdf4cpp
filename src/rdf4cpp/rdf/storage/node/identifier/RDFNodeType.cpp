#include "RDFNodeType.hpp"

namespace rdf4cpp::rdf::storage::node::identifier {

std::string as_string(RDFNodeType rdf_node_type) {
    switch (rdf_node_type) {
        case RDFNodeType::Variable:
            return "variable";
        case RDFNodeType::BNode:
            return "blank node";
        case RDFNodeType::IRI:
            return "iri";
        case RDFNodeType::Literal:
        default:
            return "literal";
    }
}
}  // namespace rdf4cpp::rdf::storage::node::identifier