#include "Literal.h"

#include "IRIResource.h"

namespace rdf4cpp::rdf::node {

Literal::Literal(void *ptr, const RDFNode::ID &id) : RDFNode(ptr, id) {}

IRIResource Literal::datatype(ResourceManager &node_manager) const {
    ID datatype_id = id_.literal().datatype_id();
    return IRIResource(node_manager.lookup_iri(datatype_id), datatype_id);
}
}  // namespace rdf4cpp::rdf::node